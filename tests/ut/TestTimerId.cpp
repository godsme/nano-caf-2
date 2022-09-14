//
// Created by Darwin Yuan on 2022/7/18.
//
#include <nano-caf/timer/TimerId.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("Empty TimerId") {
    TimerId timerId;
    REQUIRE_FALSE(timerId);
}

SCENARIO("TimerId, 0 repeat times should create an empty timer id") {
    TimerSpec spec = Duration(10000);
    TimerId timerId{0x12345678, spec, 0};
    REQUIRE_FALSE(timerId);
}

SCENARIO("TimerId Expire") {
    TimerSpec spec = Duration(10000);
    detail::TimerIdExt timerId{0x12345678, spec, 1};
    REQUIRE(timerId);

    REQUIRE(timerId.IsActive());
    REQUIRE(timerId.OnExpire() == Status::OK);
    REQUIRE((!timerId.IsActive() && !timerId.IsCancelled()));
    REQUIRE(timerId.OnExpire() == Status::CLOSED);
}

SCENARIO("Cancel TimerId") {
    TimerSpec spec = Duration(10000);
    detail::TimerIdExt timerId{0x12345678, spec, 1};
    REQUIRE(timerId);

    REQUIRE(timerId.IsActive());
    REQUIRE(timerId.Cancel(false) == Status::OK);
    REQUIRE(timerId.IsCancelled());
    REQUIRE(timerId.OnExpire() == Status::CLOSED);
}

SCENARIO("copy cons TimerId") {
    TimerSpec spec = Duration(10000);
    TimerId timerId{0x12345678, spec, 1};
    REQUIRE(timerId);

    TimerId timerId2 = timerId;
    REQUIRE(timerId2);
    REQUIRE(timerId);
}

SCENARIO("copy assign TimerId") {
    TimerSpec spec = Duration(10000);
    TimerId timerId{0x12345678, spec, 1};
    REQUIRE(timerId);

    TimerId timerId2;
    REQUIRE_FALSE(timerId2);
    timerId2 = timerId;
    REQUIRE(timerId2);
    REQUIRE(timerId);
}

SCENARIO("move cons TimerId") {
    TimerSpec spec = Duration(10000);
    TimerId timerId{0x12345678, spec, 1};
    REQUIRE(timerId);

    TimerId timerId2 = std::move(timerId);
    REQUIRE(timerId2);
    REQUIRE_FALSE(timerId);
}

SCENARIO("move assign TimerId") {
    TimerSpec spec = Duration(10000);
    TimerId timerId{0x12345678, spec, 1};
    REQUIRE(timerId);

    TimerId timerId2;
    REQUIRE_FALSE(timerId2);
    timerId2 = std::move(timerId);
    REQUIRE(timerId2);
    REQUIRE_FALSE(timerId);
}

