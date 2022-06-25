//
// Created by Darwin Yuan on 2022/6/23.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/Actor.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    CAF_def_message(Ping, (num, std::size_t));
    CAF_def_message(Pong, (num, std::size_t));
    CAF_def_message(Dead, (reason, ExitReason));

    struct PongActor : Actor {
        auto GetBehavior() noexcept -> Behavior {
            return {
                [this](Ping::Atom, std::size_t num) {
                    if(Reply<Pong>(num) != Status::OK) {
                        Exit(ExitReason::ABNORMAL);
                    }
                },
                [this](ExitMsg::Atom, ExitReason reason) {
                    if(Reply<Dead>(reason) != Status::OK) {
                        Exit(ExitReason::ABNORMAL);
                    } else {
                        Exit(reason);
                    }
                }
            };
        }
    };

    struct PingActor : Actor {
        std::size_t times{};
        ActorHandle pongActor{};

        PingActor(std::size_t times) : times{times} {}

        auto OnInit() -> void {
            pongActor = Spawn<PongActor>();
            if(!pongActor) {
                Exit(ExitReason::ABNORMAL);
            } else {
                if(Send<Ping>(pongActor, std::size_t(0)) != Status::OK) {
                    Exit(ExitReason::ABNORMAL);
                }
            }
        }

        auto GetBehavior() noexcept -> Behavior {
            return {
                [this](Pong::Atom, std::size_t num) {
                    if(num >= times) {
                        if(Send<ExitMsg>(pongActor, ExitReason::SHUTDOWN) != Status::OK) {
                            Exit(ExitReason::ABNORMAL);
                        }
                    } else if(Reply<Ping>(num + 1) != Status::OK) {
                        Exit(ExitReason::ABNORMAL);
                    }
                },
                [this](Dead::Atom, ExitReason reason) {
                    Exit(reason);
                }
            };
        }
    };
}

SCENARIO("Ping Pong BehaviorBasedActor") {
    ActorSystem::Instance().StartUp(1);

    auto ping = Spawn<PingActor, true>(std::size_t(100));
    REQUIRE(ping);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(ping.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::SHUTDOWN);

    ActorSystem::Instance().Shutdown();
}

