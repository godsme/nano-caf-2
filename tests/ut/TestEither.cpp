//
// Created by Darwin Yuan on 2022/6/24.
//
#include <nano-caf/util/Either.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("Simple Either") {
    {
        Either<int, long> either{10};
        REQUIRE(either.Left() != nullptr);
        REQUIRE(*either.Left() == 10);
        REQUIRE(either.Right() == nullptr);
        REQUIRE(either.Index() == 0);

        Either<int, long> e2 = either;

        REQUIRE(e2.Left() != nullptr);
        REQUIRE(*e2.Left() == 10);
        REQUIRE(e2.Right() == nullptr);
    }
    {
        Either<int, long> either{(long)10};
        REQUIRE(either.Left() == nullptr);
        REQUIRE(either.Right() != nullptr);
        REQUIRE(*either.Right() == 10);
        REQUIRE(either.Index() == 1);
    }
}

SCENARIO("Same Type Either") {
    {
        Either<int, int> either{EitherTag::LEFT, 10};
        REQUIRE(either.Left() != nullptr);
        REQUIRE(*either.Left() == 10);
        REQUIRE(either.Right() == nullptr);
        REQUIRE(either.Index() == 0);
    }

    {
        Either<int, int> either{EitherTag::RIGHT, 10};
        REQUIRE(either.Left() == nullptr);
        REQUIRE(either.Right() != nullptr);
        REQUIRE(*either.Right() == 10);
        REQUIRE(either.Index() == 1);
    }
}