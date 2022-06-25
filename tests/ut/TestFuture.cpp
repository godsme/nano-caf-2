//
// Created by Darwin Yuan on 2022/6/22.
//

#include <nano-caf/async/Future.h>
#include <nano-caf/async/detail/FutureObject.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("void Future") {
    Future<void> future;
    REQUIRE_FALSE(future);

    auto obj = std::make_shared<detail::FutureObject<void>>();
    future = Future<void>(obj);
    REQUIRE(future);

    bool invoked = false;
    future.Then([&invoked]() { invoked = true; });
    REQUIRE_FALSE(invoked);
    obj->SetValue();
    REQUIRE_FALSE(invoked);
    obj->Commit();
    REQUIRE(invoked);
}

SCENARIO("Future with void callback") {
    Future<int> future;
    REQUIRE_FALSE(future);

    auto obj = std::make_shared<detail::FutureObject<int>>();
    future = Future<int>(obj);
    REQUIRE(future);

    int value = 0;
    future.Then([&value](int a) { value = a; });

    obj->SetValue(101);
    REQUIRE(value == 0);

    obj->Commit();
    REQUIRE(value == 101);
}

SCENARIO("Future of Future") {
    Future<int> future;
    REQUIRE_FALSE(future);

    auto obj = std::make_shared<detail::FutureObject<int>>();
    future = Future<int>(obj);
    REQUIRE(future);

    auto obj2 = std::make_shared<detail::FutureObject<int>>();
    auto future2 = Future<int>(obj2);
    int value1 = 0;
    int value2 = 0;
    future
        .Then([&](int a) -> Future<int> { value1 = a; return future2; })
        .Then([&](int a) -> int { value2 = a; return 0; });

    obj->SetValue(101);
    obj->Commit();

    REQUIRE(value1 == 101);
    REQUIRE(value2 == 0);

    obj2->SetValue(202);
    obj2->Commit();
    REQUIRE(value1 == 101);
    REQUIRE(value2 == 202);
}

SCENARIO("Instant Future") {
    Future<int> future{10};

    int value = 0;
    future.Then([](int a) -> int {
        return a;
    }).Then([&](int a) -> void {
        value = a;
    });

    REQUIRE(value == 10);
}