//
// Created by Darwin Yuan on 2022/7/16.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <catch.hpp>
#include <spdlog/spdlog.h>

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
                spdlog::info("timeout {}", g_times);
            });
            REQUIRE(timer_1.Ok());

            timerId_1 = timer_1;

            auto timer_2 = After(10ms, [this] {
                Exit(ExitReason::ABNORMAL);
                spdlog::info("die");
            });

            REQUIRE(timer_2.Ok());

            timerId_2 = timer_2;
        }
    };
}

SCENARIO("Actor Timer") {
    spdlog::set_level(spdlog::level::info);
    g_times = 0;
    ActorSystem::Instance().StartUp(1);

    auto actor = Spawn<MyActor, true>(std::size_t(5));
    REQUIRE(actor);

    actor.Send<BootstrapMsg>();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(actor.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::ABNORMAL);

    REQUIRE(g_times == 5);

    ActorSystem::Instance().Shutdown();
}