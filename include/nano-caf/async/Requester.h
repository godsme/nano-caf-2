//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_078EE0947688427EB2DF0111C875E8F0
#define NANO_CAF_2_078EE0947688427EB2DF0111C875E8F0

#include <nano-caf/async/AbstractPromise.h>
#include <nano-caf/async/Future.h>
#include <nano-caf/util/Result.h>
#include <future>

namespace nano_caf {
    template<typename R>
    struct Requester : AbstractPromise<R> {
        using ResultType = ValueTypeOf<R>;
        using ValueType = Result<R>;

        Requester() = default;
        Requester(Requester&& another)
            : m_promise{std::move(another.m_promise)}
            , m_ready{another.m_ready} {
            another.m_ready = true;
        }

        ~Requester() {
            if(!m_ready) {
                m_promise.set_value(ValueType{ResultTag::CAUSE, Status::DISCARDED});
            }
        }

        auto GetFuture() noexcept -> std::future<ValueType> {
            return m_promise.get_future();
        }

    private:
        auto Reply(ResultType const& value, ActorWeakPtr&) noexcept -> void override {
            OnSuccess(value);
        }
        auto Join(Future<R>&& future, ActorWeakPtr&) noexcept -> void override {
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
        auto OnSuccess(ResultType const& value) noexcept -> void {
            m_promise.set_value(ValueType{ResultTag::VALUE, value});
            m_ready = true;
        }

        auto OnFail_(Status cause) noexcept -> void {
            m_promise.set_value(ValueType{ResultTag::CAUSE, cause});
            m_ready = true;
        }
    private:
        std::promise<ValueType> m_promise;
        bool m_ready{false};
    };
}

#endif //NANO_CAF_2_078EE0947688427EB2DF0111C875E8F0
