//
// Created by Darwin Yuan on 2022/6/26.
//

#ifndef NANO_CAF_2_42AEF34BED444F53990C7B61B8CC7710
#define NANO_CAF_2_42AEF34BED444F53990C7B61B8CC7710

#include <nano-caf/async/detail/FutureObserver.h>
#include <nano-caf/timer/TimerId.h>
#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf::detail {

    template<typename R>
    struct CancelTimerObserver : FutureObserver<R> {
        CancelTimerObserver(ActorWeakPtr actor, TimerId timerId)
            : m_actor{std::move(actor)}
            , m_timerId{timerId}
        {}

        using ValueType = typename FutureObserver<R>::ValueType;

        auto OnFutureReady(ValueType const&) noexcept -> void override {
            CancelTimer();
        }

        auto OnFutureFail(Status cause) noexcept -> void override {
            if(cause != Status::TIMEOUT) {
                CancelTimer();
            }
        }

    private:
        auto CancelTimer() noexcept -> void {
            auto handle = m_actor.Lock();
            if(handle) {
                ActorSystem::Instance().StopTimer((intptr_t)handle.Get(), m_timerId);
            }
        }
    private:
        ActorWeakPtr m_actor;
        TimerId m_timerId;
    };
}

#endif //NANO_CAF_2_42AEF34BED444F53990C7B61B8CC7710
