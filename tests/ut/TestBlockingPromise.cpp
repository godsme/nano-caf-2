//
// Created by Darwin Yuan on 2022/8/23.
//
#include <nano-caf/blocking/BlockingPromise.h>
#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

SCENARIO("Blocking Promise") {
    BlockingPromise<int> promise{};
    BlockingFuture<int> future = promise.GetFuture();

    auto thread = std::thread([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(2ms);
        ActorWeakPtr actor;
        static_cast<AbstractPromise<int>&>(promise).Reply(100, actor);
    });

    auto result = future.Wait();
    REQUIRE(result.Ok());
    REQUIRE(*result == 100);

    thread.join();
}

SCENARIO("Blocking Promise Fail") {
    BlockingPromise<int> promise{};
    BlockingFuture<int> future = promise.GetFuture();

    auto thread = std::thread([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(2ms);
        ActorWeakPtr actor;
        static_cast<AbstractPromise<int>&>(promise).OnFail(Status::TIMEOUT, actor);
    });

    auto result = future.Wait();
    REQUIRE_FALSE(result.Ok());
    REQUIRE(result.GetStatus() == Status::TIMEOUT);

    thread.join();
}

SCENARIO("Blocking Promise Discard") {
    BlockingPromise<int> promise{};
    BlockingFuture<int> future = promise.GetFuture();

    auto thread = std::thread([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(2ms);
    });

    auto result = future.Wait();
    REQUIRE_FALSE(result.Ok());
    REQUIRE(result.GetStatus() == Status::DISCARDED);

    thread.join();
}