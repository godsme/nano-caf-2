//
// Created by Darwin Yuan on 2022/7/1.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/msg/RequestDef.h>
#include <catch.hpp>


using namespace nano_caf;

namespace {
    enum : uint32_t {
        MSG_interface_id = 2
    };

    __CAF_actor_interface(Msg, MSG_interface_id,
        (Open,  (const long&) -> long),
        (Close, () -> void)
    );

    struct MyActor : Actor {
        auto GetBehavior() noexcept -> Behavior {
            return {
                [](Msg::Open, long value) -> long {
                    return 10 + value;
                },
                [this](Msg::Close) -> void {
                    Exit(ExitReason::NORMAL);
                },
            };
        }
    };
}

SCENARIO("Blocking Actor") {
    auto actor = SpawnBlockingActor<MyActor>();
    auto result = actor.Request<Msg::Open>(22);
    REQUIRE(result.Ok());
    REQUIRE(*result == 32);

    REQUIRE(actor.Request<Msg::Close>().Ok());

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
}

SCENARIO("Blocking Actor Send") {
    auto actor = SpawnBlockingActor<MyActor>();
    auto result = actor.Request<Msg::Open>(22);
    REQUIRE(result.Ok());
    REQUIRE(*result == 32);

    actor.Send<Msg::Close>();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
}