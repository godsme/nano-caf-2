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