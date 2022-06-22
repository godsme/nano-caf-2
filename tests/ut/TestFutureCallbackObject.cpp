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
    OnActorContext context;
    REQUIRE(context.Empty());
    std::weak_ptr<detail::FutureObject<int>> weakSubject;
    Observer observer1, observer2;
    Status cause{Status::OK};

    auto callback = [](int a) -> std::string {
        return (a > 100) ? "201" : "101";
    };

    using ObjectType = detail::FutureCallbackObject<std::string, int, decltype(callback)>;
    std::weak_ptr<ObjectType> weak;
    {
        auto subject = std::make_shared<detail::FutureObject<int>>(context);
        weakSubject = subject;
        auto obj = std::make_shared<ObjectType>(context, subject, std::move(callback));
        weak = obj;
        context.AddObject(obj);
        obj->RegisterObserver(observer1);
        obj->RegisterObserver(observer2);
        obj->SetFailHandler([&](Status reason) {
            cause = reason;
        });
    }

    REQUIRE(weakSubject.use_count() == 1);
    REQUIRE(weak.use_count() == 1);

    WHEN("set value on subject") {
        {
            auto subject = weakSubject.lock();
            subject->SetValue(101);
            REQUIRE(observer1.result.index() == 0);
            REQUIRE(observer2.result.index() == 0);

            THEN("once committed, all observer should be notified") {
                subject->Commit();

                REQUIRE(observer1.result.index() == 1);
                REQUIRE(observer2.result.index() == 1);
                REQUIRE(std::get<1>(observer1.result) == "201");
                REQUIRE(std::get<1>(observer2.result) == "201");

                REQUIRE(cause == Status::OK);

                THEN("callback object should be deregistered") {
                    REQUIRE(context.Empty());
                    REQUIRE(weak.expired());
                }
            }
        }
        REQUIRE(weakSubject.expired());
    }

    WHEN("fail") {
        {
            auto subject = weakSubject.lock();
            subject->OnFail(Status::CLOSED);
            REQUIRE(observer1.result.index() == 0);
            REQUIRE(observer2.result.index() == 0);

            THEN("once committed, all observer should be notified") {
                subject->Commit();

                REQUIRE(observer1.result.index() == 2);
                REQUIRE(observer2.result.index() == 2);
                REQUIRE(std::get<2>(observer1.result) == Status::CLOSED);
                REQUIRE(std::get<2>(observer2.result) == Status::CLOSED);

                REQUIRE(cause == Status::CLOSED);

                THEN("callback object should be deregistered") {
                    REQUIRE(context.Empty());
                    REQUIRE(weak.expired());
                }
            }
        }
        REQUIRE(weakSubject.expired());
    }
}