//
// Created by Darwin Yuan on 2022/8/22.
//
#include <nano-caf/blocking/Future.h>
#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

SCENARIO("Blocking Future") {
    auto object = std::make_shared<blocking::detail::FutureObject<int>>();
    blocking::Future<int> future{object};

    auto thread = std::thread([object]{
        std::this_thread::sleep_for(2ms);
        object->SetValue(100);
    });

    auto result = future.Wait();
    REQUIRE(result.Ok());
    REQUIRE(*result == 100);

    thread.join();
}

SCENARIO("Blocking Future Wait For") {
    auto object = std::make_shared<blocking::detail::FutureObject<int>>();
    blocking::Future<int> future{object};

    auto thread = std::thread([object]{
        std::this_thread::sleep_for(10ms);
        object->SetValue(100);
    });

    WHEN("Wait For longer") {
        auto result = future.WaitFor(20ms);
        REQUIRE(result.Ok());
        REQUIRE(*result == 100);
    }

    WHEN("Wait For shorter") {
        auto result = future.WaitFor(1ms);
        REQUIRE_FALSE(result.Ok());
        REQUIRE(result.GetStatus() == Status::TIMEOUT);
    }
    thread.join();
}

SCENARIO("Empty Blocking Future") {
    blocking::Future<int> future{};

    WHEN("Wait") {
        auto result = future.Wait();
        REQUIRE_FALSE(result.Ok());
        REQUIRE(result.GetStatus() == Status::NULL_PTR);
    }

    WHEN("Set callback") {
        future.Then([](auto&& result) {
            REQUIRE_FALSE(result.Ok());
            REQUIRE(result.GetStatus() == Status::NULL_PTR);
        });
    }
}

