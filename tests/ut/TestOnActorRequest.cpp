//
// Created by Darwin Yuan on 2022/6/24.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/msg/RequestDef.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    enum : uint32_t {
        MSG_interface_id = 1
    };

    __CAF_actor_interface(Msg, MSG_interface_id,
        (Open,  (const long&) -> long),
        (Close, (const long&) -> void)
    );

    struct MyActor : Actor {
        Status cause{Status::OK};
        long result = 0;
        auto Test() {
            ActorHandle to;
            Request<Msg::Open>(to, 100)
                    .Then([this](long r) {
                        result = r;
                        Exit(ExitReason::NORMAL);
                    })
                    .Fail([this](auto&& cause) {
                        this->cause = cause;
                    });
        }
        auto HandleMessage(Message &msg) noexcept -> void {}
    };
}

SCENARIO("OnActorRequest") {
    detail::InternalSchedActor<MyActor> actor{false};
    actor.Test();
    REQUIRE(actor.cause == Status::NULL_ACTOR);
}

namespace {
    struct ServerActor : Actor {
        long base{0};
        ServerActor(long base) : base{base} {}

        const Behavior INIT_Behavior {
            [this](Msg::Open, long value) -> long {
                return value + base;
            },
            [this](ExitMsg::Atom, ExitReason reason) {
                Exit(reason);
            }
        };
    };

    long result = 0;
    struct RequestActor : Actor {
        ActorHandle server{};

        long value{};
        RequestActor(long value) : value{value} {}

        auto OnInit() -> void {
            server = Spawn<ServerActor>(101);
            if(!server) {
                Exit(ExitReason::ABNORMAL);
            } else {
                Request<Msg::Open>(server, value)
                    .Then([this](long r) {
                        result = r;
                        Exit(ExitReason::NORMAL);
                    })
                    .Fail([this](auto&&) {
                        Exit(ExitReason::ABNORMAL);
                    });
            }
        }
    };
}

SCENARIO("OnActorRequest calc") {
    ActorSystem::Instance().StartUp(2);

    auto requester = Spawn<RequestActor, true>(203);
    REQUIRE(requester);

    requester.Send<BootstrapMsg>();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(result == 203 + 101);

    ActorSystem::Instance().Shutdown();
}