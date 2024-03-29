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
                            (Open, (const long&) -> long),
                            (Close,(const long&) -> void)
    );

    CAF_def_message(DoneNotify, (num, long));

}

namespace {
    long finalResult = 0;
    struct ServerActor : Actor {
        ServerActor(long base, bool waitNotify) : base{base}, waitNotify{waitNotify} {}

        const Behavior INIT_Behavior {
            [this](Msg::Open, long value) -> Future<long> {
                if(waitNotify) {
                    return ExpectMsg<DoneNotify>([this, value](auto&& notify) -> long {
                        return base + value + notify.num;
                    });
                } else {
                    return base + value;
                }
            },
            [this](Msg::Close, long value) -> Future<void> {
                if(waitNotify) {
                    return ExpectMsg<DoneNotify>([this, value](auto&& notify) -> void {
                        finalResult = base + value + notify.num;
                    });
                } else {
                    finalResult = base + value;
                    return Void;
                }
            },
            [this](ExitMsg::Atom, ExitReason reason) {
                Exit(reason);
            }
        };

    private:
        long base{0};
        bool waitNotify{};
    };

    long result = 0;
    struct RequestActor : Actor {
        ActorPtr server{};
        long value{};

        RequestActor(long value, ActorPtr server)
            : server{server}
            , value{value}
        {}

        auto OnActorInit() -> void {
            using namespace std::chrono_literals;
            Request<Msg::Open>(server, 2000ms, value)
                .Then([this](long r) {
                    result = r;
                    Exit(ExitReason::NORMAL);
                })
                .Fail([this](Status) {
                    Exit(ExitReason::ABNORMAL);
                });
        }
    };
}

SCENARIO("On Actor Expect Msg") {
    ActorSystem::Instance().StartUp(1);

    result = 0;

    auto server = Spawn<ServerActor>(101, true);
    REQUIRE(server);

    auto requester = Spawn<RequestActor, true>(203, server);
    REQUIRE(requester);

    GlobalActorContext::Send<BootstrapMsg>(requester);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    GlobalActorContext::Send<DoneNotify>(server, 20);
    server.Release();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(result == 203 + 101 + 20);

    ActorSystem::Instance().Shutdown();
}

SCENARIO("On Actor No Wait") {
    ActorSystem::Instance().StartUp(1);

    result = 0;

    auto server = Spawn<ServerActor>(101, false);
    REQUIRE(server);

    auto requester = Spawn<RequestActor, true>(203, server);
    REQUIRE(requester);

    server.Release();

    GlobalActorContext::Send<BootstrapMsg>(requester);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(result == 203 + 101);

    ActorSystem::Instance().Shutdown();
}

namespace {
    struct CloseRequestActor : Actor {
        ActorPtr server{};
        long value{};

        CloseRequestActor(long value, ActorPtr server)
            : server{server}
            , value{value}
        {}

        auto OnActorInit() -> void {
            Request<Msg::Close>(server, value)
                .Then([this]() {
                    Exit(ExitReason::NORMAL);
                })
                .Fail([this](Status) {
                    Exit(ExitReason::ABNORMAL);
                });
        }
    };
}

using namespace std::chrono_literals;

SCENARIO("On Actor Wait void") {
    ActorSystem::Instance().StartUp(1);

    finalResult = 0;

    auto server = Spawn<ServerActor>(101, true);
    REQUIRE(server);

    auto requester = Spawn<CloseRequestActor, true>(203, server);
    REQUIRE(requester);

    GlobalActorContext::Send<BootstrapMsg>(requester);

    std::this_thread::sleep_for(10ms);
    GlobalActorContext::Send<DoneNotify>(server, 20);
    server.Release();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(finalResult == 203 + 101 + 20);

    ActorSystem::Instance().Shutdown();
}

SCENARIO("On Actor no Wait void") {
    ActorSystem::Instance().StartUp(1);

    finalResult = 0;

    auto server = Spawn<ServerActor>(101, false);
    REQUIRE(server);

    auto requester = Spawn<CloseRequestActor, true>(203, server);
    REQUIRE(requester);

    server.Release();

    GlobalActorContext::Send<BootstrapMsg>(requester);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(finalResult == 203 + 101);

    ActorSystem::Instance().Shutdown();
}

namespace {
    struct ServerActor2 : Actor {
        ServerActor2(long base) : base{base}{}

        auto GetBehavior() -> Behavior {
            return {
                    [this](Msg::Open, long value) -> Future<long> {
                        return ExpectMsg<DoneNotify>(1000ms, [value, this](auto &&notify) -> long {
                            return base + value + notify.num;
                        });
                    },
                    [this](Msg::Close, long) -> Future<void> {
                        return ExpectMsg<DoneNotify>(1000ms, [](auto&&) -> void {});
                    },
                    [this](ExitMsg::Atom, ExitReason reason) {
                        Exit(reason);
                    }
            };
        }

    private:
        long base{0};
    };

    Status failedCause = Status::OK;
    struct RequestActor2 : Actor {
        ActorPtr server{};
        long value{};

        RequestActor2(long value, ActorPtr server)
                : server{server}
                , value{value}
        {}

        auto OnActorInit() -> void {
            Request<Msg::Open>(server, value)
                    .Then([this](long r) {
                        result = r;
                        Exit(ExitReason::NORMAL);
                    })
                    .Fail([this](Status cause) {
                        failedCause = cause;
                        Exit(ExitReason::ABNORMAL);
                    });
        }
    };
}

SCENARIO("On Actor expect msg timeout ") {
    failedCause = Status::OK;
    ActorSystem::Instance().StartUp(1);

    result = 0;

    auto server = Spawn<ServerActor2>(101);
    REQUIRE(server);

    auto requester = Spawn<RequestActor2, true>(203, server);
    REQUIRE(requester);

    server.Release();

    GlobalActorContext::Send<BootstrapMsg>(requester);


    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);
    REQUIRE(result == 0);
    REQUIRE(failedCause == Status::TIMEOUT);

    ActorSystem::Instance().Shutdown();
}

SCENARIO("On Actor expect msg without timeout ") {
    failedCause = Status::OK;
    ActorSystem::Instance().StartUp(1);

    result = 0;

    auto server = Spawn<ServerActor2>(101);
    REQUIRE(server);

    auto requester = Spawn<RequestActor2, true>(203, server);
    REQUIRE(requester);

    GlobalActorContext::Send<BootstrapMsg>(requester);

    std::this_thread::sleep_for(10ms);
    GlobalActorContext::Send<DoneNotify>(server, 20);
    server.Release();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(result == 203 + 101 + 20);
    REQUIRE(failedCause == Status::OK);

    ActorSystem::Instance().Shutdown();
}