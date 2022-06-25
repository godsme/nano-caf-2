//
// Created by Darwin Yuan on 2022/6/21.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    struct Ping {
        constexpr static MsgTypeId ID = 11;
        std::size_t num;
    };

    struct Pong {
        constexpr static MsgTypeId ID = 12;
        std::size_t num;
    };

    struct Dead {
        constexpr static MsgTypeId ID = 13;
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
                case ExitMsg::ID: {
                    auto reason = msg.Body<ExitMsg>()->reason;
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
                if(Send<Ping>(pongActor, std::size_t(0)) != Status::OK) {
                    Exit(ExitReason::ABNORMAL);
                }
            }
        }

        auto HandleMessage(Message& msg) noexcept -> void {
            switch (msg.id) {
                case Pong::ID: {
                    auto num = msg.Body<Pong>()->num;
                    if(num >= times) {
                        if(Send<ExitMsg>(pongActor, ExitReason::SHUTDOWN) != Status::OK) {
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

SCENARIO("Ping Pong Actor") {
    ActorSystem::Instance().StartUp(1);

    auto ping = Spawn<PingActor, true>(std::size_t(100));
    REQUIRE(ping);

    ping.Send<BootstrapMsg>();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(ping.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::SHUTDOWN);

    ActorSystem::Instance().Shutdown();
}
