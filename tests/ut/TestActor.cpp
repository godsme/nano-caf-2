//
// Created by Darwin Yuan on 2022/6/23.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/msg/RequestDef.h>
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

using namespace std::chrono_literals;

namespace {
    enum : uint32_t {
        MSG_interface_id = 2
    };

    __CAF_actor_interface(Msg, MSG_interface_id,
        (Open, (const long&) -> unsigned long),
        (Seek, (const long&) -> unsigned long),
        (Discard, () -> void),
        (Close, () -> void)
    );

    struct ServerActor : Actor {
        const Behavior INIT_Behavior {
            [](Msg::Open, long value) -> unsigned long {
                return value + 100;
            },
            [this](Msg::Seek, long value) -> Future<unsigned long> {
                return After(2ms, [value] () -> unsigned long {
                   return 100 + value;
                });
            },
            [this](Msg::Close) -> Future<void> {
                return After(100ms, []{});
            }
        };
    };

    ExitReason serverReason;

    struct ClientActor : Actor {
        ActorPtr server{};

        auto OnInit() -> void {
            server = Spawn<ServerActor, true>();
        }

        auto OnExit() -> void {
            ActorWeakPtr weak = server;
            server.Release();
            weak.Wait(serverReason);
        }

        const Behavior INIT_Behavior {
                [this](Msg::Open, long value) -> Future<unsigned long> {
                    return Request<Msg::Open>(server, 10ms, value + 10);
                },
                [this](Msg::Seek, long value) -> Future<unsigned long> {
                    return Request<Msg::Seek>(server, 10ms, value + 10);
                },
                [this](Msg::Discard) -> Future<void> {
                    return Request<Msg::Discard>(server, 10ms);
                },
                [this](Msg::Close) -> Future<void> {
                    return Request<Msg::Close>(server, 10ms);
                }
        };
    };
}

SCENARIO("Actor Timeout Request") {
    serverReason = ExitReason::UNKNOWN;
    ActorSystem::Instance().StartUp(1);

    ActorWeakPtr weakClient;
    {
        auto client = Spawn<ClientActor, true>();
        REQUIRE(client);

        weakClient = client;

        {
            auto result = GlobalActorContext::Request<Msg::Open>(client, 20);
            REQUIRE(result.Ok());
            REQUIRE(*result == 130);
        }

        {
            auto result = GlobalActorContext::Request<Msg::Discard>(client);
            REQUIRE_FALSE(result.Ok());
            REQUIRE(result.GetStatus() == Status::DISCARDED);
        }

        {
            auto result = GlobalActorContext::Request<Msg::Seek>(client, 30);
            REQUIRE(result.Ok());
            REQUIRE(*result == 140);
        }

        {
            auto result = GlobalActorContext::Request<Msg::Close>(client);
            REQUIRE_FALSE(result.Ok());
            REQUIRE(result.GetStatus() == Status::TIMEOUT);
        }
    }

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(weakClient.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);
    REQUIRE(serverReason == ExitReason::ABNORMAL);

    ActorSystem::Instance().Shutdown();
}

