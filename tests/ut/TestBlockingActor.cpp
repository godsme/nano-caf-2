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
        (View,  (const long&) -> long),
        (Close, () -> void)
    );

    struct Server : Actor {
        auto GetBehavior() noexcept -> Behavior {
            return {
                [](Msg::View, long value) -> long {
                    return 100 + value;
                },
                [this](Msg::Close) -> void {
                    Exit(ExitReason::NORMAL);
                }
            };
        }
    };
    struct MyActor : Actor {
        ActorHandle server{};

        auto OnInit() -> void {
            server = SpawnBlockingActor<Server, false>();
        }

        auto GetBehavior() noexcept -> Behavior {
            return {
                [](Msg::Open, long value) -> long {
                    return 10 + value;
                },
                [this](Msg::View, long value) -> Future<long> {
                    return Future<long>::Forward(ForwardTo(server));
                },
                [this](Msg::Close) -> void {
                    Request<Msg::Close>(server)
                        .Then([this] { Exit(ExitReason::NORMAL); });
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

    result = actor.Request<Msg::View>(22);
    REQUIRE(result.Ok());
    REQUIRE(*result == 122);

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