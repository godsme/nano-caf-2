//
// Created by Darwin Yuan on 2022/9/15.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/msg/RequestDef.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    std::size_t num = 0;
    struct SimpleActor : Actor {
        auto Process() -> void {
            if(++num == 100) {
                Exit(ExitReason::SHUTDOWN);
            } else {
                Resched();
            }
        }

        auto Resched() -> void {
            Async<Message::URGENT>([this]{ Process(); });
        }

        auto OnActorInit() -> void {
            Resched();
        }
    };
}

SCENARIO("Actor Async Reschedule") {
    num = 0;
    auto actor = SpawnBlockingActor<SimpleActor>();
    REQUIRE(actor);

    ExitReason reason{ExitReason::NORMAL};
    actor.Wait(reason);

    REQUIRE(reason == ExitReason::SHUTDOWN);
    REQUIRE(num == 100);
}