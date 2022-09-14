//
// Created by Darwin Yuan on 2022/7/1.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/msg/RequestDef.h>
#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

namespace {
    enum : uint32_t {
        MSG_interface_id  = 2,
        MSG1_interface_id = 3
    };

    __CAF_actor_interface(Msg, MSG_interface_id,
        (Open,    (const long&) -> unsigned long),
        (View,    (const long&) -> long),
        (Discard, () -> void),
        (Close,   () -> void)
    );

    struct Server : Actor {
        auto GetBehavior() noexcept -> Behavior {
            return {
                [](Msg::Open, long value) -> unsigned long {
                    std::this_thread::sleep_for(100ms);
                    return 10 + value;
                },
                [](Msg::View, long value) -> long {
                    return 100 + value;
                },
                [this](Msg::Close) -> void {
                    Exit(ExitReason::NORMAL);
                }
            };
        }
    };

    bool exit_invoked = false;
    struct MyActor : Actor {
        ActorPtr server{};

        auto OnActorInit() -> void {
            exit_invoked = false;
            server = SpawnBlockingActor<Server, false>();
        }

        auto OnActorExit() -> void {
            exit_invoked = true;
        }

        auto GetBehavior() noexcept -> Behavior {
            return {
                [this](Msg::Open, long value) -> Future<unsigned long> {
                    return Request<Msg::Open>(server, value);
                },
                [this](Msg::View, long) -> Future<long> {
                    return ForwardTo<long, Message::URGENT>(server);
                },
                [this](Msg::Close) -> Future<void> {
                    return Request<Msg::Close>(server)
                              .Then([this] { Exit(ExitReason::NORMAL); });
                },
            };
        }
    };
}

SCENARIO("Blocking Actor") {
    auto actor = SpawnBlockingActor<MyActor>();
    auto result = GlobalActorContext::Request<Msg::Open>(actor, 22);
    REQUIRE(result.Ok());
    REQUIRE(*result == 32);

    auto result2 = GlobalActorContext::Request<Msg::View>(actor, 22);
    REQUIRE(result2.Ok());
    REQUIRE(*result2 == 122);

    REQUIRE(GlobalActorContext::Request<Msg::Close>(actor).Ok());

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
}

SCENARIO("Blocking Actor Send") {
    auto actor = SpawnBlockingActor<MyActor>();
    auto result = GlobalActorContext::Request<Msg::Open>(actor, 22);
    REQUIRE(result.Ok());
    REQUIRE(*result == 32);

    GlobalActorContext::Send<Msg::Close>(actor);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);

    REQUIRE(exit_invoked);
}

SCENARIO("Blocking Actor No closing") {
    ActorWeakPtr weak{};
    {
        auto actor = SpawnBlockingActor<MyActor>();
        auto result = GlobalActorContext::Request<Msg::Open>(actor, 22);
        REQUIRE(result.Ok());
        REQUIRE(*result == 32);

        weak = actor;
    }

    REQUIRE(exit_invoked);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(weak.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);
}

SCENARIO("Blocking Actor Unimplemented") {
    ActorWeakPtr weak{};
    {
        auto actor = SpawnBlockingActor<MyActor>();
        auto result = GlobalActorContext::Request<Msg::Discard>(actor);
        REQUIRE_FALSE(result.Ok());
        REQUIRE(result.GetStatus() == Status::DISCARDED);

        weak = actor;
    }

    REQUIRE(exit_invoked);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(weak.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);
}

namespace {
    __CAF_actor_interface(Msg1, MSG1_interface_id,
        (Seek,  (const long&) -> long)
    );

    struct BaseActor : Actor {
        const Behavior InitBehavior = Behavior {
            [](Msg::Open, long value) -> unsigned long {
                return 20 + value;
            },
            [](Msg::View, long value) -> Future<long> {
                return 21 + value;
            },
        };

        auto GetBehavior() noexcept -> Behavior {
            return {
                [](Msg::Open, long value) -> unsigned long {
                    return 10 + value;
                },
                [](Msg::View, long value) -> Future<long> {
                    return 11 + value;
                },
                [this](Msg::Close) -> void {
                    Exit(ExitReason::NORMAL);
                },
            };
        }
    };

    struct DerivedActor : BaseActor {
        auto GetBehavior() noexcept -> Behavior {
            return BaseActor::GetBehavior() + Behavior {
                [](Msg1::Seek, long value) -> long {
                    return 12 + value;
                },
                [](Msg::View, long value) -> long {
                    return 13 + value;
                },
            };
        }
    };
}

SCENARIO("Blocking Actor Derived GetBehavior") {
    auto actor = SpawnBlockingActor<DerivedActor>();
    auto result = GlobalActorContext::Request<Msg::Open>(actor, 22);
    REQUIRE(result.Ok());
    REQUIRE(*result == 32);

    auto result2 = GlobalActorContext::Request<Msg1::Seek>(actor, 10);
    REQUIRE(result2.Ok());
    REQUIRE(*result2 == 22);

    result2 = GlobalActorContext::Request<Msg::View>(actor, 10);
    REQUIRE(result2.Ok());
    REQUIRE(*result2 == 21);

    GlobalActorContext::Send<Msg::Close>(actor);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
}

namespace {
    struct DerivedActor2 : BaseActor {
        auto GetBehavior() noexcept -> Behavior {
            return Behavior {
                    [](Msg1::Seek, long value) -> long {
                        return 12 + value;
                    },
                    [](Msg::View, long value) -> long {
                        return 13 + value;
                    },
            } + BaseActor::GetBehavior();
        }
    };
}
SCENARIO("Blocking Actor Derived GetBehavior Override") {
    auto actor = SpawnBlockingActor<DerivedActor2>();
    {
        auto result = GlobalActorContext::Request<Msg::Open>(actor, 22);
        REQUIRE(result.Ok());
        REQUIRE(*result == 32);
    }

    {
        auto result = GlobalActorContext::Request<Msg1::Seek>(actor, 10);
        REQUIRE(result.Ok());
        REQUIRE(*result == 22);
    }

    {
        auto result = GlobalActorContext::Request<Msg::View>(actor, 10);
        REQUIRE(result.Ok());
        REQUIRE(*result == 23);
    }

    GlobalActorContext::Send<Msg::Close>(actor);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
}