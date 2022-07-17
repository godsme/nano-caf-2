//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_C601717AA6D74F32826C395175572D97
#define NANO_CAF_2_C601717AA6D74F32826C395175572D97

#include <nano-caf/async/AbstractPromise.h>
#include <nano-caf/async/Future.h>
#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/msg/PredefinedMsgs.h>

namespace nano_caf {
    template<typename T>
    struct Promise : AbstractPromise<T> {
        using Object = std::shared_ptr<detail::FutureObject<T>>;

        Promise() : m_future{std::make_shared<detail::FutureObject<T>>()} {}
        explicit Promise(Status cause) : Promise{} {
            if(m_future) m_future->OnFail(cause);
        }

        auto GetFuture() noexcept -> Object {
            return m_future;
        }

        static auto GetFailedFuture(Status status) -> Object {
            return Promise{status}.m_future;
        }

    private:
        auto OnFail(Status cause, ActorWeakPtr& to) noexcept -> void override {
            if(m_future && m_future->OnFail(cause)) {
                Notify(to);
            }
        }

        auto Join(Future<T>&& future, ActorWeakPtr& to) noexcept -> void override {
            if constexpr(std::is_void_v<T>) {
                future.Then([promise = *this, to = to] () mutable {
                    promise.Reply(Void, to);
                });
            } else {
                future.Then([promise = *this, to = to](T const& value) mutable {
                    promise.Reply(value, to);
                });
            }

            future.Fail([promise = *this, to = to](Status cause) mutable {
                promise.OnFail(cause, to);
            });
        }

        auto Reply(ValueTypeOf<T> const& value, ActorWeakPtr& to) noexcept -> void override {
            if(m_future && m_future->SetValue(value)) {
                Notify(to);
            }
        }

    private:
        auto Notify(ActorWeakPtr& to) noexcept -> void {
            auto actor = to.Lock();
            if(actor) {
                ActorHandle{std::move(actor)}.Send<FutureDoneNotify>(std::move(m_future));
            }
        }
    private:
        Object m_future;
    };
}

#endif //NANO_CAF_2_C601717AA6D74F32826C395175572D97
