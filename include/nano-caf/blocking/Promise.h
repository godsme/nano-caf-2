//
// Created by Darwin Yuan on 2022/8/22.
//

#ifndef NANO_CAF_2_5089EB00FB8B4400B36D50EE57D6344E
#define NANO_CAF_2_5089EB00FB8B4400B36D50EE57D6344E

#include <nano-caf/async/AbstractPromise.h>
#include <nano-caf/blocking/Future.h>

namespace nano_caf::blocking {
    template<typename R>
    struct Promise : AbstractPromise<R> {
        using ResultType = ValueTypeOf<R>;
        using FutureObject = blocking::detail::FutureObject<R>;

        Promise() : m_object{std::make_shared<FutureObject>()} {}
        Promise(Promise&& another)
                : m_object{std::move(another.m_object)} {
            another.m_object = nullptr;
        }

        ~Promise() {
            if(m_object && !m_object->HasBeenSet()) {
                OnFail_(Status::DISCARDED);
            }
        }

        explicit operator bool () {
            return m_object;
        }

        auto GetFuture() noexcept -> blocking::Future<R> {
            return {m_object};
        }

    private:
        auto Reply(ResultType const& value, ActorWeakPtr&) noexcept -> void override {
            OnSuccess(value);
        }

        auto Reply(ResultType&& value, ActorWeakPtr&) noexcept -> void override {
            OnSuccess(std::move(value));
        }

        auto Join(nano_caf::Future<R>&& future, ActorWeakPtr&) noexcept -> void override {
            if(future.IsForward()) return;

            future.Fail([this](Status cause) {
                OnFail_(cause);
            });

            if constexpr(std::is_void_v<R>) {
                future.Then([this]() {
                    OnSuccess(Void);
                });
            } else {
                future.Then([this](auto&& value) {
                    OnSuccess(value);
                });
            }
        }

        auto OnFail(Status cause, ActorWeakPtr&) noexcept -> void override {
            OnFail_(cause);
        }

    private:
        auto OnFail_(Status cause) noexcept -> void {
            CAF_ASSERT_VALID_PTR_VOID(m_object);
            CAF_ASSERT_VOID(m_object->OnFail(cause));
        }

        auto OnSuccess(ResultType const& value) noexcept -> void {
            CAF_ASSERT_VALID_PTR_VOID(m_object);
            CAF_ASSERT_VOID(m_object->SetValue(value));
        }

        auto OnSuccess(ResultType&& value) noexcept -> void {
            CAF_ASSERT_VALID_PTR_VOID(m_object);
            CAF_ASSERT_VOID(m_object->SetValue(std::move(value)));
        }

    private:
        std::shared_ptr<FutureObject> m_object;
    };
}

#endif //NANO_CAF_2_5089EB00FB8B4400B36D50EE57D6344E
