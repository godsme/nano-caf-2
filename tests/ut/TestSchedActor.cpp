//
// Created by Darwin Yuan on 2022/6/17.
//
#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/SharedPtr.h>
#include <nano-caf/msg/Message.h>

#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

namespace {
    struct MySchedActor : SchedActor {
        virtual auto Init() noexcept -> void override {
            inited = true;
            initTimes++;
        }

        virtual auto UserDefinedHandleMessage(Message& msg) noexcept -> void override {
            REQUIRE(initTimes == 1);
            msgs[numOfMsgs++] = msg.id;
            if(msg.id == 10) SchedActor::Exit_(ExitReason::SHUTDOWN);
        }

        auto ExitHandler(ExitReason reason) noexcept -> void override {
            exitReason = reason;
        }

        bool inited{false};
        MsgTypeId msgs[10];
        size_t numOfMsgs{0};
        size_t initTimes{0};
        ExitReason exitReason{ExitReason::NORMAL};
    };

    bool claimed = false;
    struct MemClaimer {
        static auto Free(void* p) noexcept -> void {
            claimed = true;
            ::operator delete(p);
        }
    };

    using Actor = SharedBlock<MySchedActor, MemClaimer>;

    std::size_t allocatedBlocks = 0;
    struct MyMessage : Message {
        using Message::Message;

        auto operator new(std::size_t size) noexcept -> void* {
            allocatedBlocks++;
            return ::operator new(size);
        }

        auto operator delete(void* p) noexcept -> void {
            allocatedBlocks--;
            ::operator delete(p);
        }

        auto GetBody() const noexcept -> void const* override {
            return nullptr;
        }
    };
}

SCENARIO("SchedActor Resume") {
    claimed = false;
    auto* rawActor = (new Actor{})->Get();
    {
        auto actor = SharedPtr<MySchedActor>(rawActor, false);

        REQUIRE(actor->SendMsg(new MyMessage{1, Message::Category::NORMAL}) == MailBox::Result::BLOCKED);
        REQUIRE(actor->SendMsg(new MyMessage{2, Message::Category::URGENT}) == MailBox::Result::OK);
        REQUIRE(allocatedBlocks == 2);

        auto *task = static_cast<Resumable *>(actor.Get());
        REQUIRE(task->Resume() == TaskResult::DONE);
        REQUIRE(actor->inited);
        REQUIRE(actor->initTimes == 1);
        REQUIRE(actor->numOfMsgs == 2);
        REQUIRE(actor->msgs[0] == 2);
        REQUIRE(actor->msgs[1] == 1);
        REQUIRE(allocatedBlocks == 0);

        actor->numOfMsgs = 0;

        REQUIRE(actor->SendMsg(new MyMessage{3, Message::Category::NORMAL}) == MailBox::Result::BLOCKED);
        REQUIRE(actor->SendMsg(new MyMessage{4, Message::Category::URGENT}) == MailBox::Result::OK);
        REQUIRE(actor->SendMsg(new MyMessage{5, Message::Category::NORMAL}) == MailBox::Result::OK);
        REQUIRE(actor->SendMsg(new MyMessage{6, Message::Category::URGENT}) == MailBox::Result::OK);
        REQUIRE(allocatedBlocks == 4);

        REQUIRE(task->Resume() == TaskResult::SUSPENDED);
        REQUIRE(actor->initTimes == 1);
        REQUIRE(actor->numOfMsgs == 3);
        REQUIRE(actor->msgs[0] == 4);
        REQUIRE(actor->msgs[1] == 6);
        REQUIRE(actor->msgs[2] == 3);
        REQUIRE(allocatedBlocks == 1);

        actor->numOfMsgs = 0;
        REQUIRE(actor->SendMsg(new MyMessage{7, Message::Category::URGENT}) == MailBox::Result::OK);
        REQUIRE(task->Resume() == TaskResult::DONE);
        REQUIRE(actor->initTimes == 1);
        REQUIRE(actor->numOfMsgs == 2);
        REQUIRE(actor->msgs[0] == 7);
        REQUIRE(actor->msgs[1] == 5);
        REQUIRE(allocatedBlocks == 0);

        actor->numOfMsgs = 0;
        REQUIRE(actor->SendMsg(new MyMessage{8, Message::Category::NORMAL}) == MailBox::Result::BLOCKED);
        REQUIRE(actor->SendMsg(new MyMessage{9, Message::Category::NORMAL}) == MailBox::Result::OK);
        REQUIRE(actor->SendMsg(new MyMessage{10, Message::Category::URGENT}) == MailBox::Result::OK);
        REQUIRE(task->Resume() == TaskResult::DONE);
        REQUIRE(actor->initTimes == 1);
        REQUIRE(actor->numOfMsgs == 1);
        REQUIRE(actor->msgs[0] == 10);
        REQUIRE(allocatedBlocks == 0);
        REQUIRE(actor->exitReason == ExitReason::SHUTDOWN);

        REQUIRE(actor->SendMsg(new MyMessage{11, Message::Category::NORMAL}) == MailBox::Result::CLOSED);

        REQUIRE_FALSE(claimed);
    }
    REQUIRE(claimed);
}