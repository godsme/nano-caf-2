//
// Created by Darwin Yuan on 2022/8/22.
//

#ifndef NANO_CAF_2_5CF232FA52F4448EBCD8D5F2E8762F1B
#define NANO_CAF_2_5CF232FA52F4448EBCD8D5F2E8762F1B

#include <nano-caf/async/AbstractPromise.h>
#include <nano-caf/blocking/BlockingFuture.h>
#include <nano-caf/async/Future.h>

namespace nano_caf {
    template<typename R>
    struct BlockingPromise : AbstractPromise<R> {
        using ResultType = ValueTypeOf<R>;
        using FutureObject = detail::BlockingFutureObject<R>;

        BlockingPromise() : m_object{std::make_shared<FutureObject>()} {}
        BlockingPromise(BlockingPromise const& another) : m_object{another.m_object}, m_owner{false} {}
        BlockingPromise(BlockingPromise&& another) : m_object{std::move(another.m_object)}, m_owner{another.m_owner} {
            another.m_owner = false;
            another.m_object = nullptr;
        }

        auto operator=(BlockingPromise const& rhs) noexcept -> BlockingPromise& = delete;
        auto operator=(BlockingPromise&& rhs) noexcept -> BlockingPromise& = delete;

        ~BlockingPromise() {
            if(m_owner && m_object) {
                m_object->NotifyDiscard();
            }
        }

        explicit operator bool () {
            return m_object;
        }

        auto GetFuture() noexcept -> BlockingFuture<R> {
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

            future.Fail([promise = *this](Status cause) mutable {
                promise.OnFail_(cause);
            });

            if constexpr(std::is_void_v<R>) {
                future.Then([promise = std::move(*this)]() mutable {
                    promise.OnSuccess(Void);
                });
            } else {
                future.Then([promise = std::move(*this)](auto&& value) mutable {
                    promise.OnSuccess(value);
                });
            }
        }

        auto OnFail(Status cause, ActorWeakPtr&) noexcept -> void override {
            OnFail_(cause);
        }

    private:
        auto OnFail_(Status cause) noexcept -> void {
            CAF_ASSERT__(m_object);
            CAF_ASSERT__(m_object->OnFail(cause));
        }

        auto OnSuccess(ResultType const& value) noexcept -> void {
            CAF_ASSERT__(m_object);
            CAF_ASSERT__(m_object->SetValue(value));
        }

        auto OnSuccess(ResultType&& value) noexcept -> void {
            CAF_ASSERT__(m_object);
            CAF_ASSERT__(m_object->SetValue(std::move(value)));
        }

    private:
        std::shared_ptr<FutureObject> m_object;
        bool m_owner{true};
    };
}

#endif //NANO_CAF_2_5CF232FA52F4448EBCD8D5F2E8762F1B
