//
// Created by Darwin Yuan on 2022/6/23.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    CAF_def_message(Ping, (num, std::size_t));
    CAF_def_message(Pong, (num, std::size_t));
    CAF_def_message(Dead, (reason, ExitReason));

    struct PongActor : Actor {
        const Behavior INIT_Behavior = {
                [this](Ping::Atom, std::size_t num) {
                    Reply<Pong>(num);
                },
                [this](ExitMsg::Atom, ExitReason reason) {
                    Reply<Dead>(reason);
                    Exit(reason);
                }
        };
    };

    struct PingActor : Actor {
        std::size_t times{};
        ActorPtr pongActor{};

        PingActor(std::size_t times) : times{times} {}

        auto OnInit() -> void {
            pongActor = Spawn<PongActor>();
            Send<Ping>(pongActor, std::size_t(0));
        }

        const Behavior INIT_Behavior {
                [this](Pong::Atom, std::size_t num) {
                    if(num < times) {
                        Reply<Ping>(num + 1);
                    } else {
                        Send<ExitMsg>(pongActor, ExitReason::SHUTDOWN);
                    }
                },
                [this](Dead::Atom, ExitReason reason) {
                    Exit(reason);
                }
        };
    };
}

SCENARIO("Ping Pong BehaviorBasedActor") {
    ActorSystem::Instance().StartUp(1);

    auto ping = Spawn<PingActor, true>(std::size_t(10));
    REQUIRE(ping);

    GlobalActorContext::Send<BootstrapMsg>(ping);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(ping.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::SHUTDOWN);

    ActorSystem::Instance().Shutdown();
}

