//
// Created by Darwin Yuan on 2022/6/21.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    struct MyActor : Actor {
        MyActor(int a, int b) : a{a}, b{b} {}
        int a{}, b{};
    };

    struct MyMessage {
        constexpr static MsgTypeId TYPE_ID = 1;
        int a;
        int b;
    };
}
SCENARIO("Actor") {
    ActorSystem::Instance().StartUp(1);
    auto actor = Spawn<MyActor>(1, 2);
    REQUIRE(actor.Send<MyMessage>(10, 20) == Status::OK);
    ActorSystem::Instance().Shutdown();
}