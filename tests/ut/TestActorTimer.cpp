//
// Created by Darwin Yuan on 2022/7/16.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

namespace {
    std::size_t g_times = 0;
    struct MyActor : Actor {
        TimerId timerId_1{};
        TimerId timerId_2{};
        std::size_t repeatTimes{};

        MyActor(std::size_t repeatTimes) : repeatTimes{repeatTimes} {}

        auto OnInit() -> void {
            auto timer_1 = Repeat(1ms, repeatTimes, []{
                g_times++;
            });
            REQUIRE(timer_1.Ok());

            timerId_1 = *timer_1;

            auto timer_2 = After(10ms, [this] {
                Exit(ExitReason::ABNORMAL);
            });

            REQUIRE(timer_2.Ok());

            timerId_2 = *timer_2;
        }
    };
}

SCENARIO("Actor Timer") {
    g_times = 0;
    ActorSystem::Instance().StartUp(1);

    auto actor = Spawn<MyActor, true>(std::size_t(5));
    REQUIRE(actor);

    GlobalActorContext::Send<BootstrapMsg>(actor);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);

    REQUIRE(g_times == 5);

    ActorSystem::Instance().Shutdown();
}

namespace {
    struct MyActor2 : Actor {
        TimerId timerId_1{};
        TimerId timerId_2{};

        auto OnInit() -> void {
            auto timer_1 = Repeat(1ms, []{
                g_times++;
            });
            REQUIRE(timer_1.Ok());

            timerId_1 = *timer_1;

            auto timer_2 = After(3ms, [this] {
                timerId_1.Cancel();
            });

            REQUIRE(timer_2.Ok());

            timerId_2 = *timer_2;

            After(5ms, [this] {
                Exit(ExitReason::ABNORMAL);
            });
        }
    };
}

SCENARIO("Actor Timer Cancel") {
    g_times = 0;
    ActorSystem::Instance().StartUp(1);

    auto actor = Spawn<MyActor2, true>();
    REQUIRE(actor);

    GlobalActorContext::Send<BootstrapMsg>(actor);

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);

    REQUIRE(g_times == 3);

    ActorSystem::Instance().Shutdown();
}