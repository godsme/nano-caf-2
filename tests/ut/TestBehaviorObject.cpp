//
// Created by Darwin Yuan on 2022/6/23.
//
#include <nano-caf/actor/detail/BehaviorObject.h>
#include <nano-caf/msg/MsgTypeId.h>
#include <nano-caf/msg/MakeMessage.h>
#include <nano-caf/msg/MsgDef.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    struct Msg1 {
        constexpr static MsgTypeId ID = 101;
        int a;
        int b;
    };
}

SCENARIO("non atom behavior object") {
    auto* msg = MakeMessage<Msg1>(10, 20);
    bool invoked = false;
    auto f = [&invoked](Msg1 const& msg) -> void {
        REQUIRE(msg.a == 10);
        REQUIRE(msg.b == 20);
        invoked = true;
    };

    detail::BehaviorObject<decltype(f)> obj(std::move(f));

    REQUIRE(obj(*msg));
    REQUIRE(invoked);
}

namespace {
    CAF_def_message(Msg2, (a, int), (b, int));
}

SCENARIO("atom behavior object") {
    auto* msg = MakeMessage<Msg2>(10, 20);
    bool invoked = false;
    auto f = [&invoked](Msg2_atom, int a, int b) -> void {
        REQUIRE(a == 10);
        REQUIRE(b == 20);
        invoked = true;
    };

    detail::BehaviorObject<decltype(f)> obj(std::move(f));

    REQUIRE(obj(*msg));
    REQUIRE(invoked);
}