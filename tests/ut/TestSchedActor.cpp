//
// Created by Darwin Yuan on 2022/6/17.
//
#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/SharedPtr.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>

#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

namespace {
    struct MySchedActor : SchedActor {
        auto Send(Message* msg) noexcept -> Status override {
            return MailBox::SendMsg(msg);
        }

        virtual auto HandleUserDefinedMsg(Message& msg) noexcept -> bool override {
            msgs[numOfMsgs++] = msg.id;
            if(msg.id == 1000) SchedActor::Exit_(ExitReason::SHUTDOWN);
            return true;
        }

        auto ExitHandler(ExitReason reason) noexcept -> void override {
            exitReason = reason;
        }

        MsgTypeId msgs[10];
        size_t numOfMsgs{0};
        ExitReason exitReason{ExitReason::NORMAL};
    };

    bool claimed = false;
    struct MemClaimer {
        static auto Free(void* p) noexcept -> void {
            claimed = true;
            ::operator delete(p);
        }
    };

    using Actor = SharedBlock<MySchedActor, detail::ActorCtlBlock, MemClaimer>;

    std::vector<MsgTypeId> discardMessages;

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

        REQUIRE(actor->Send(new MyMessage{100, Message::Category::NORMAL}) == Status::BLOCKED);
        REQUIRE(actor->Send(new MyMessage{200, Message::Category::URGENT}) == Status::OK);
        REQUIRE(allocatedBlocks == 2);

        auto *task = static_cast<SchedActor *>(actor.Get());
        REQUIRE(task->Resume(3) == TaskResult::DONE);
        REQUIRE(actor->numOfMsgs == 2);
        REQUIRE(actor->msgs[0] == 200);
        REQUIRE(actor->msgs[1] == 100);
        REQUIRE(allocatedBlocks == 0);

        actor->numOfMsgs = 0;

        REQUIRE(actor->Send(new MyMessage{300, Message::Category::NORMAL}) == Status::BLOCKED);
        REQUIRE(actor->Send(new MyMessage{400, Message::Category::URGENT}) == Status::OK);
        REQUIRE(actor->Send(new MyMessage{500, Message::Category::NORMAL}) == Status::OK);
        REQUIRE(actor->Send(new MyMessage{600, Message::Category::URGENT}) == Status::OK);
        REQUIRE(allocatedBlocks == 4);

        REQUIRE(task->Resume(3) == TaskResult::SUSPENDED);
        REQUIRE(actor->numOfMsgs == 3);
        REQUIRE(actor->msgs[0] == 400);
        REQUIRE(actor->msgs[1] == 600);
        REQUIRE(actor->msgs[2] == 300);
        REQUIRE(allocatedBlocks == 1);

        actor->numOfMsgs = 0;
        REQUIRE(actor->Send(new MyMessage{700, Message::Category::URGENT}) == Status::OK);
        REQUIRE(task->Resume(3) == TaskResult::DONE);
        REQUIRE(actor->numOfMsgs == 2);
        REQUIRE(actor->msgs[0] == 700);
        REQUIRE(actor->msgs[1] == 500);
        REQUIRE(allocatedBlocks == 0);

        actor->numOfMsgs = 0;
        REQUIRE(actor->Send(new MyMessage{800, Message::Category::NORMAL}) == Status::BLOCKED);
        REQUIRE(actor->Send(new MyMessage{900, Message::Category::NORMAL}) == Status::OK);
        REQUIRE(actor->Send(new MyMessage{1000, Message::Category::URGENT}) == Status::OK);
        REQUIRE(task->Resume(3) == TaskResult::DONE);
        REQUIRE(actor->numOfMsgs == 1);
        REQUIRE(actor->msgs[0] == 1000);
        REQUIRE(allocatedBlocks == 0);
        REQUIRE(actor->exitReason == ExitReason::SHUTDOWN);

        REQUIRE(actor->Send(new MyMessage{11, Message::Category::NORMAL}) == Status::CLOSED);

        REQUIRE_FALSE(claimed);
    }
    REQUIRE(claimed);
}

namespace {
    struct MySchedActor2 : SchedActor {
        auto Send(Message* msg) noexcept -> Status override {
            return MailBox::SendMsg(msg);
        }

        auto ExitHandler(ExitReason reason) noexcept -> void override {
            exitReason = reason;
        }
        ExitReason exitReason{ExitReason::NORMAL};
    };

    using Actor2 = SharedBlock<MySchedActor2, detail::ActorCtlBlock, MemClaimer>;
}

namespace {
    struct Msg2 : MyMessage {
        using MyMessage::MyMessage;
        auto OnDiscard() noexcept -> void override {
            discardMessages.push_back(Message::id);
        }
    };
}

SCENARIO("SchedActor Destroy") {
    discardMessages.clear();
    auto actor = SharedPtr<MySchedActor2>((new Actor2{})->Get(), false);

    REQUIRE(actor->Send(new Msg2{100, Message::Category::NORMAL}) == Status::BLOCKED);
    REQUIRE(actor->Send(new MyMessage{200, Message::Category::URGENT}) == Status::OK);
    REQUIRE(actor->Send(new Msg2{300, Message::Category::URGENT}) == Status::OK);
    REQUIRE(allocatedBlocks == 3);
    REQUIRE(discardMessages.size() == 0);
    actor.Release();
    REQUIRE(discardMessages.size() == 2);
    REQUIRE(discardMessages[0] == 300);
    REQUIRE(discardMessages[1] == 100);
}