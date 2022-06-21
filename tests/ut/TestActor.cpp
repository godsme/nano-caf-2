//
// Created by Darwin Yuan on 2022/6/21.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <catch.hpp>
#include <iostream>

using namespace nano_caf;

namespace {
    struct Ping {
        constexpr static MsgTypeId ID = 1;
        std::size_t num;
    };

    struct Pong {
        constexpr static MsgTypeId ID = 2;
        std::size_t num;
    };

    struct Shutdown {
        constexpr static MsgTypeId ID = 3;
        ExitReason reason;
    };

    struct Dead {
        constexpr static MsgTypeId ID = 4;
        ExitReason reason;
    };

    struct PongActor : Actor {
        auto HandleMessage(Message& msg) noexcept -> void {
            switch (msg.id) {
                case Ping::ID: {
                    auto num = msg.Body<Ping>()->num;
                    if(Reply<Pong>(num) != Status::OK) {
                        Exit(ExitReason::ABNORMAL);
                    }
                    break;
                }
                case Shutdown::ID: {
                    auto reason = msg.Body<Shutdown>()->reason;
                    if(Reply<Dead>(reason) != Status::OK) {
                        Exit(ExitReason::ABNORMAL);
                    } else {
                        Exit(reason);
                    }
                }
            }
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
                if(SendTo<Ping>(pongActor, std::size_t(0)) != Status::OK) {
                    Exit(ExitReason::ABNORMAL);
                }
            }
        }

        auto HandleMessage(Message& msg) noexcept -> void {
            switch (msg.id) {
                case Pong::ID: {
                    auto num = msg.Body<Pong>()->num;
                    if(num >= times) {
                        if(SendTo<Shutdown>(pongActor, ExitReason::SHUTDOWN) != Status::OK) {
                            Exit(ExitReason::ABNORMAL);
                        }
                    } else if(Reply<Ping>(num + 1) != Status::OK) {
                        Exit(ExitReason::ABNORMAL);
                    }
                    break;
                }
                case Dead::ID: {
                    Exit(msg.Body<Dead>()->reason);
                    break;
                }
            }
        }
    };
}

SCENARIO("Actor") {
    ActorSystem::Instance().StartUp(1);
    auto ping = Spawn<PingActor, true>(std::size_t(100));
    REQUIRE(ping);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(ping.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::SHUTDOWN);

    ActorSystem::Instance().Shutdown();
}