//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_C601717AA6D74F32826C395175572D97
#define NANO_CAF_2_C601717AA6D74F32826C395175572D97

#include <nano-caf/async/AbstractPromise.h>
#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/msg/PredefinedMsgs.h>

namespace nano_caf {
    template<typename T>
    struct Promise : AbstractPromise<T> {
        auto GetFuture() noexcept -> Future<T> {
            if(!m_future) {
                m_future = std::make_shared<detail::FutureObject<T>>();
            }
            return {m_future};
        }

    private:
        auto OnFail(Status cause, ActorWeakPtr& to) noexcept -> void override {
            if(!m_future) return;
            m_future->OnFail(cause);
            SendBack(to);
        }

        auto Join(Future<T>&& future, ActorWeakPtr& to) noexcept -> void override {
            if constexpr(std::is_void_v<T>) {
                future.Then([this, to = to] {
                    Reply(std::move(Void::Instance()), to);
                });
            } else {
                future.Then([this, to = to](T const& value) {
                    Reply(value, to);
                });
            }

            future.Fail([this, to = to](Status cause) {
                OnFail(cause, to);
            });;
        }

        auto Reply(ValueTypeOf<T>&& value, ActorWeakPtr& to) noexcept -> void override {
            if(!m_future) return;
            m_future->SetValue(value);
            SendBack(to);
        }

    private:
        auto SendBack(ActorWeakPtr& to) noexcept -> void {
            auto actor = to.Lock();
            if(actor) {
                ActorHandle{std::move(actor)}.Send<FutureDoneMsg>(m_future);
            }
        }
    private:
        std::shared_ptr<detail::FutureObject<T>> m_future;
    };
}

#endif //NANO_CAF_2_C601717AA6D74F32826C395175572D97
