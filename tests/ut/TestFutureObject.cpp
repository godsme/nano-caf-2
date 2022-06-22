//
// Created by Darwin Yuan on 2022/6/22.
//
#include <nano-caf/async/detail/FutureObject.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    struct Observer : detail::FutureObserver<int> {
        virtual auto OnFutureReady(int const& value) noexcept -> void {
            result.emplace<1>(value);
        }
        virtual auto OnFutureFail(Status cause) noexcept -> void {
            result.emplace<2>(cause);
        }

        std::variant<std::monostate, int, Status> result;
    };
}

SCENARIO("FutureObject destruct") {
    auto observer1 = std::make_shared<Observer>();
    auto observer2 = std::make_shared<Observer>();
    REQUIRE(observer1.use_count() == 1);
    REQUIRE(observer2.use_count() == 1);

    {
        auto obj = std::make_shared<detail::FutureObject<int>>();
        obj->RegisterObserver(observer1);
        obj->RegisterObserver(observer2);

        REQUIRE(observer1.use_count() == 2);
        REQUIRE(observer2.use_count() == 2);
    }

    REQUIRE(observer1.use_count() == 1);
    REQUIRE(observer2.use_count() == 1);
}

SCENARIO("int FutureObject") {
    auto observer1 = std::make_shared<Observer>();
    auto observer2 = std::make_shared<Observer>();

    auto obj = std::make_shared<detail::FutureObject<int>>();

    REQUIRE(observer1.use_count() == 1);
    REQUIRE(observer2.use_count() == 1);

    obj->RegisterObserver(observer1);
    obj->RegisterObserver(observer2);

    REQUIRE(observer1.use_count() == 2);
    REQUIRE(observer2.use_count() == 2);

    REQUIRE(obj.use_count() == 1);

    WHEN("future failed, if not commit, observers should not be notified") {
        obj->OnFail(Status::CLOSED);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        REQUIRE(observer1.use_count() == 2);
        REQUIRE(observer2.use_count() == 2);

        THEN("once commit, all observers should be notified") {
            obj->Commit();
            REQUIRE(observer1->result.index() == 2);
            REQUIRE(observer2->result.index() == 2);
            REQUIRE(std::get<2>(observer1->result) == Status::CLOSED);
            REQUIRE(std::get<2>(observer2->result) == Status::CLOSED);
            THEN("after commit, observers should be cleared") {
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }

            THEN("if register a new observer, it should be notified immediately") {
                auto observer3 = std::make_shared<Observer>();
                REQUIRE(observer3->result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3->result.index() == 2);
                REQUIRE(std::get<2>(observer3->result) == Status::CLOSED);
            }
        }
    }

    WHEN("future succeed, if not commit, observers should not be notified") {
        obj->SetValue(101);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once commit, all observers should be notified") {
            obj->Commit();
            REQUIRE(observer1->result.index() == 1);
            REQUIRE(observer2->result.index() == 1);
            REQUIRE(std::get<1>(observer1->result) == 101);
            REQUIRE(std::get<1>(observer2->result) == 101);

            THEN("after commit, observers should be cleared") {
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }

            THEN("if register a new observer, it should be notified immediately") {
                auto observer3 = std::make_shared<Observer>();
                REQUIRE(observer3->result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3->result.index() == 1);
                REQUIRE(std::get<1>(observer3->result) == 101);
            }
        }
    }
}

namespace {
    struct VoidObserver : detail::FutureObserver<void> {
        virtual auto OnFutureReady(Void const& value) noexcept -> void {
            result.emplace<1>(value);
        }
        virtual auto OnFutureFail(Status cause) noexcept -> void {
            result.emplace<2>(cause);
        }

        std::variant<std::monostate, Void, Status> result;
    };
}

SCENARIO("void FutureObject") {
    auto observer1 = std::make_shared<VoidObserver>();
    auto observer2 = std::make_shared<VoidObserver>();

    auto obj = std::make_shared<detail::FutureObject<void>>();

    REQUIRE(observer1.use_count() == 1);
    REQUIRE(observer2.use_count() == 1);

    obj->RegisterObserver(observer1);
    obj->RegisterObserver(observer2);

    REQUIRE(observer1.use_count() == 2);
    REQUIRE(observer2.use_count() == 2);

    REQUIRE(obj.use_count() == 1);

    WHEN("future failed, if not commit, observers should not be notified") {
        obj->OnFail(Status::CLOSED);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once commit, all observers should be notified") {
            obj->Commit();
            REQUIRE(observer1->result.index() == 2);
            REQUIRE(observer2->result.index() == 2);
            REQUIRE(std::get<2>(observer1->result) == Status::CLOSED);
            REQUIRE(std::get<2>(observer2->result) == Status::CLOSED);
            THEN("after commit, it should be deregistered from registry") {
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }

            THEN("if register a new observer, it should be notified immediately") {
                auto observer3 = std::make_shared<VoidObserver>();
                REQUIRE(observer3->result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3->result.index() == 2);
                REQUIRE(std::get<2>(observer3->result) == Status::CLOSED);
            }
        }
    }

    WHEN("future succeed, if not commit, observers should not be notified") {
        obj->SetValue();
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once commit, all observers should be notified") {
            obj->Commit();
            REQUIRE(observer1->result.index() == 1);
            REQUIRE(observer2->result.index() == 1);

            THEN("after commit, it should be deregistered from registry") {
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }

            THEN("if register a new observer, it should be notified immediately") {
                auto observer3 = std::make_shared<VoidObserver>();
                REQUIRE(observer3->result.index() == 0);
                REQUIRE(observer3->result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3->result.index() == 1);
                REQUIRE(observer3.use_count() == 1);
            }
        }
    }
}