//
// Created by Darwin Yuan on 2022/6/26.
//
#include <nano-caf/timer/ActorTimerSystem.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("ActorTimerSystem") {
    ActorTimerSystem timerSystem;

    timerSystem.Start();
    timerSystem.Stop();
}