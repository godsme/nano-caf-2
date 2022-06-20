//
// Created by Darwin Yuan on 2022/6/20.
//
#include <nano-caf/util/SharedPtr.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("SharedPtr") {
    auto p = MakeShared<int>(10);
    REQUIRE((bool)p);
    REQUIRE(*p == 10);
}