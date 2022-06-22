//
// Created by Darwin Yuan on 2022/6/22.
//
#include <nano-caf/async/detail/FutureCallbackObject.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    struct Observer : detail::FutureObserver<std::string> {
        virtual auto OnFutureReady(std::string const& value) noexcept -> void {
            result.emplace<1>(value);
        }
        virtual auto OnFutureFail(Status cause) noexcept -> void {
            result.emplace<2>(cause);
        }

        std::variant<std::monostate, std::string, Status> result;
    };
}

SCENARIO("int FutureCallbackObject") {
    auto observer1 = std::make_shared<Observer>();
    auto observer2 = std::make_shared<Observer>();
    Status cause{Status::OK};

    auto callback = [](int a) -> std::string {
        return (a > 100) ? "201" : "101";
    };

    using ObjectType = detail::FutureCallbackObject<std::string, int, decltype(callback)>;
    auto subject = std::make_shared<detail::FutureObject<int>>();
    auto obj = std::make_shared<ObjectType>(std::move(callback));
    subject->RegisterObserver(obj);
    REQUIRE(obj.use_count() == 2);

    obj->RegisterObserver(observer1);
    obj->RegisterObserver(observer2);

    REQUIRE(observer1.use_count() == 2);
    REQUIRE(observer2.use_count() == 2);

    obj->SetFailHandler([&](Status reason) {
        cause = reason;
    });

    WHEN("set value on subject") {
        subject->SetValue(101);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once committed, all observer should be notified") {
            subject->Commit();

            REQUIRE(observer1->result.index() == 1);
            REQUIRE(observer2->result.index() == 1);
            REQUIRE(std::get<1>(observer1->result) == "201");
            REQUIRE(std::get<1>(observer2->result) == "201");

            REQUIRE(cause == Status::OK);

            THEN("callback object should be deregistered") {
                REQUIRE(obj.use_count() == 1);
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }
        }
    }

    WHEN("fail") {
        subject->OnFail(Status::CLOSED);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once committed, all observer should be notified") {
            subject->Commit();

            REQUIRE(observer1->result.index() == 2);
            REQUIRE(observer2->result.index() == 2);
            REQUIRE(std::get<2>(observer1->result) == Status::CLOSED);
            REQUIRE(std::get<2>(observer2->result) == Status::CLOSED);

            REQUIRE(cause == Status::CLOSED);

            THEN("callback object should be deregistered") {
                REQUIRE(obj.use_count() == 1);
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
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

SCENARIO("void FutureCallbackObject") {
    auto observer1 = std::make_shared<VoidObserver>();
    auto observer2 = std::make_shared<VoidObserver>();
    Status cause{Status::OK};
    int value = 0;

    auto callback = [&value](int a) { value = a; };

    using ObjectType = detail::FutureCallbackObject<void, int, decltype(callback)>;
    auto subject = std::make_shared<detail::FutureObject<int>>();
    auto obj = std::make_shared<ObjectType>(std::move(callback));
    subject->RegisterObserver(obj);
    REQUIRE(obj.use_count() == 2);

    obj->RegisterObserver(observer1);
    obj->RegisterObserver(observer2);

    REQUIRE(observer1.use_count() == 2);
    REQUIRE(observer2.use_count() == 2);

    obj->SetFailHandler([&](Status reason) {
        cause = reason;
    });

    WHEN("set value on subject") {
        subject->SetValue(101);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once committed, all observer should be notified") {
            REQUIRE(value == 0);
            subject->Commit();

            REQUIRE(value == 101);
            REQUIRE(observer1->result.index() == 1);
            REQUIRE(observer2->result.index() == 1);

            REQUIRE(cause == Status::OK);

            THEN("callback object should be deregistered") {
                REQUIRE(obj.use_count() == 1);
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }
        }
    }

    WHEN("fail") {
        subject->OnFail(Status::CLOSED);
        REQUIRE(observer1->result.index() == 0);
        REQUIRE(observer2->result.index() == 0);

        THEN("once committed, all observer should be notified") {
            subject->Commit();

            REQUIRE(observer1->result.index() == 2);
            REQUIRE(observer2->result.index() == 2);
            REQUIRE(std::get<2>(observer1->result) == Status::CLOSED);
            REQUIRE(std::get<2>(observer2->result) == Status::CLOSED);

            REQUIRE(cause == Status::CLOSED);
            REQUIRE(value == 0);

            THEN("callback object should be deregistered") {
                REQUIRE(obj.use_count() == 1);
                REQUIRE(observer1.use_count() == 1);
                REQUIRE(observer2.use_count() == 1);
            }
        }
    }
}