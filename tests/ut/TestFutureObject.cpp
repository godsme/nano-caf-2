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

SCENARIO("int FutureObject") {
    OnActorContext context;
    REQUIRE(context.Empty());
    Observer observer1, observer2;

    std::weak_ptr<detail::FutureObject<int>> weak;
    {
        auto obj = std::make_shared<detail::FutureObject<int>>(context);
        context.AddObject(obj);
        weak = obj;
        REQUIRE(weak.use_count() == 2);
        obj->RegisterObserver(observer1);
        obj->RegisterObserver(observer2);
    }

    REQUIRE(weak.use_count() == 1);

    WHEN("future failed, if not commit, observers should not be notified") {
        auto obj = weak.lock();
        obj->OnFail(Status::CLOSED);
        REQUIRE(observer1.result.index() == 0);
        REQUIRE(observer2.result.index() == 0);

        THEN("once commit, all observers should be notified") {
            REQUIRE_FALSE(context.Empty());
            obj->Commit();
            REQUIRE(observer1.result.index() == 2);
            REQUIRE(observer2.result.index() == 2);
            REQUIRE(std::get<2>(observer1.result) == Status::CLOSED);
            REQUIRE(std::get<2>(observer2.result) == Status::CLOSED);
            THEN("after commit, it should be deregistered from registry") {
                REQUIRE(context.Empty());
            }

            THEN("if register a new observer, it should be notified immediately") {
                Observer observer3;
                REQUIRE(observer3.result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3.result.index() == 2);
                REQUIRE(std::get<2>(observer3.result) == Status::CLOSED);
            }
        }
    }

    WHEN("future succeed, if not commit, observers should not be notified") {
        auto obj = weak.lock();
        obj->SetValue(101);
        REQUIRE(observer1.result.index() == 0);
        REQUIRE(observer2.result.index() == 0);

        THEN("once commit, all observers should be notified") {
            REQUIRE_FALSE(context.Empty());
            obj->Commit();
            REQUIRE(observer1.result.index() == 1);
            REQUIRE(observer2.result.index() == 1);
            REQUIRE(std::get<1>(observer1.result) == 101);
            REQUIRE(std::get<1>(observer2.result) == 101);

            THEN("after commit, it should be deregistered from registry") {
                REQUIRE(context.Empty());
            }

            THEN("if register a new observer, it should be notified immediately") {
                Observer observer3;
                REQUIRE(observer3.result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3.result.index() == 1);
                REQUIRE(std::get<1>(observer3.result) == 101);
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
    OnActorContext context;
    REQUIRE(context.Empty());
    VoidObserver observer1, observer2;

    std::weak_ptr<detail::FutureObject<void>> weak;
    {
        auto obj = std::make_shared<detail::FutureObject<void>>(context);
        context.AddObject(obj);
        weak = obj;
        REQUIRE(weak.use_count() == 2);
        obj->RegisterObserver(observer1);
        obj->RegisterObserver(observer2);
    }

    REQUIRE(weak.use_count() == 1);

    WHEN("future failed, if not commit, observers should not be notified") {
        auto obj = weak.lock();
        obj->OnFail(Status::CLOSED);
        REQUIRE(observer1.result.index() == 0);
        REQUIRE(observer2.result.index() == 0);

        THEN("once commit, all observers should be notified") {
            REQUIRE_FALSE(context.Empty());
            obj->Commit();
            REQUIRE(observer1.result.index() == 2);
            REQUIRE(observer2.result.index() == 2);
            REQUIRE(std::get<2>(observer1.result) == Status::CLOSED);
            REQUIRE(std::get<2>(observer2.result) == Status::CLOSED);
            THEN("after commit, it should be deregistered from registry") {
                REQUIRE(context.Empty());
            }

            THEN("if register a new observer, it should be notified immediately") {
                VoidObserver observer3;
                REQUIRE(observer3.result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3.result.index() == 2);
                REQUIRE(std::get<2>(observer3.result) == Status::CLOSED);
            }
        }
    }

    WHEN("future succeed, if not commit, observers should not be notified") {
        auto obj = weak.lock();
        obj->SetValue();
        REQUIRE(observer1.result.index() == 0);
        REQUIRE(observer2.result.index() == 0);

        THEN("once commit, all observers should be notified") {
            REQUIRE_FALSE(context.Empty());
            obj->Commit();
            REQUIRE(observer1.result.index() == 1);
            REQUIRE(observer2.result.index() == 1);

            THEN("after commit, it should be deregistered from registry") {
                REQUIRE(context.Empty());
            }

            THEN("if register a new observer, it should be notified immediately") {
                VoidObserver observer3;
                REQUIRE(observer3.result.index() == 0);
                obj->RegisterObserver(observer3);
                REQUIRE(observer3.result.index() == 1);
            }
        }
    }
}