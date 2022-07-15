//
// Created by Darwin Yuan on 2022/6/24.
//
#include <nano-caf/actor/detail/ExpectOnceMsgHandlers.h>
#include <nano-caf/msg/MakeMessage.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    std::vector<std::size_t> vector;

    struct Handler : detail::CancellableMsgHandler {
        Handler(int id) : id{id} {}
        auto Cancel() noexcept -> void override {}
        auto OnTimeout() noexcept -> bool override { return true; }
        auto HandleMsg(Message&) noexcept -> bool override {
            vector.push_back(id);
            return true;
        }

        int id{0};
    };

    struct Msg1 {
        constexpr static MsgTypeId ID = 1;
    };

    struct Msg2 {
        constexpr static MsgTypeId ID = 2;
    };

    struct Msg3 {
        constexpr static MsgTypeId ID = 3;
    };

    struct Msg4 {
        constexpr static MsgTypeId ID = 4;
    };
}

SCENARIO("ExpectOnceMsgHandlers") {
    detail::ExpectOnceMsgHandlers handlers;
    REQUIRE(handlers.Empty());

    handlers.AddHandler(1, std::make_shared<Handler>(1));
    handlers.AddHandler(2, std::make_shared<Handler>(3));
    handlers.AddHandler(3, std::make_shared<Handler>(5));
    handlers.AddHandler(1, std::make_shared<Handler>(2));
    handlers.AddHandler(2, std::make_shared<Handler>(4));

    REQUIRE(!handlers.Empty());

    {
        auto msg4 = std::unique_ptr<Message>(MakeMessage<Msg4>());
        REQUIRE(!handlers.HandleMsg(*msg4));
        REQUIRE(vector.empty());
    }

    {
        auto msg3 = std::unique_ptr<Message>(MakeMessage<Msg3>());
        REQUIRE(handlers.HandleMsg(*msg3));
        REQUIRE(vector.size() == 1);
        REQUIRE(vector[0] == 5);
        vector.clear();
        REQUIRE(!handlers.Empty());
        REQUIRE(!handlers.HandleMsg(*msg3));
        REQUIRE(vector.size() == 0);
    }

    {
        auto msg2 = std::unique_ptr<Message>(MakeMessage<Msg2>());
        REQUIRE(handlers.HandleMsg(*msg2));
        REQUIRE(vector.size() == 2);
        REQUIRE((vector[0] == 3 && vector[1] == 4));
        vector.clear();
        REQUIRE(!handlers.HandleMsg(*msg2));
        REQUIRE(vector.size() == 0);
    }

    {
        auto msg1 = std::unique_ptr<Message>(MakeMessage<Msg1>());
        REQUIRE(handlers.HandleMsg(*msg1));
        REQUIRE(vector.size() == 2);
        REQUIRE((vector[0] == 1 && vector[1] == 2));
        vector.clear();
        REQUIRE(!handlers.HandleMsg(*msg1));
        REQUIRE(vector.size() == 0);
    }

    REQUIRE(handlers.Empty());
}