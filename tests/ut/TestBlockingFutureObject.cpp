//
// Created by Darwin Yuan on 2022/8/22.
//
#include <nano-caf/blocking/detail/FutureObject.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("Blocking FutureObject") {
    blocking::detail::FutureObject<int> object;

    std::optional<Result<int>> result;
    REQUIRE(object.SetCallback([&](Result<int>&& r) {
        result.template emplace(r);
    }) == Status::OK);

    REQUIRE_FALSE(result);

    WHEN("Set callback again") {
        REQUIRE(object.SetCallback([&](Result<int>&& r) {
            result.template emplace(r);
        }) == Status::INVALID_OP);
    }

    WHEN("Set value") {
        REQUIRE(object.SetValue(100) == Status::OK);

        REQUIRE(result);
        REQUIRE(result.value() == 100);

        WHEN("Set value again") {
            REQUIRE(object.SetValue(101) == Status::INVALID_OP);
        }

        WHEN("On Fail") {
            REQUIRE(object.OnFail(Status::OUT_OF_MEM) == Status::INVALID_OP);
        }
    }

    WHEN("On fail") {
        REQUIRE(object.OnFail(Status::OUT_OF_MEM) == Status::OK);

        REQUIRE(result);
        REQUIRE(result->GetStatus() == Status::OUT_OF_MEM);

        WHEN("On Fail again") {
            REQUIRE(object.OnFail(Status::OUT_OF_MEM) == Status::INVALID_OP);
        }

        WHEN("Set value") {
            REQUIRE(object.SetValue(101) == Status::INVALID_OP);
        }
    }
}

SCENARIO("Blocking FutureObject Set callback later") {
    blocking::detail::FutureObject<int> object;

    std::optional<Result<int>> result;

    WHEN("Set value") {
        REQUIRE(object.SetValue(100) == Status::OK);

        REQUIRE(object.SetCallback([&](Result<int>&& r) {
            result.template emplace(r);
        }) == Status::OK);

        REQUIRE(result);
        REQUIRE(result.value() == 100);

        WHEN("Set value again") {
            REQUIRE(object.SetValue(101) == Status::INVALID_OP);
        }

        WHEN("On Fail") {
            REQUIRE(object.OnFail(Status::OUT_OF_MEM) == Status::INVALID_OP);
        }

        WHEN("Set callback again") {
            REQUIRE(object.SetCallback([&](Result<int>&& r) {
                result.template emplace(r);
            }) == Status::INVALID_OP);
        }
    }

    WHEN("On fail") {
        REQUIRE(object.OnFail(Status::OUT_OF_MEM) == Status::OK);

        REQUIRE(object.SetCallback([&](Result<int>&& r) {
            result.template emplace(r);
        }) == Status::OK);

        REQUIRE(result);
        REQUIRE(result->GetStatus() == Status::OUT_OF_MEM);

        WHEN("On Fail again") {
            REQUIRE(object.OnFail(Status::OUT_OF_MEM) == Status::INVALID_OP);
        }

        WHEN("Set value") {
            REQUIRE(object.SetValue(101) == Status::INVALID_OP);
        }
    }
}