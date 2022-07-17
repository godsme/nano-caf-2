//
// Created by Darwin Yuan on 2022/6/26.
//

#ifndef NANO_CAF_2_42AEF34BED444F53990C7B61B8CC7710
#define NANO_CAF_2_42AEF34BED444F53990C7B61B8CC7710

#include <nano-caf/async/detail/FutureObserver.h>
#include <nano-caf/timer/TimerId.h>

namespace nano_caf::detail {

    template<typename R>
    struct CancelTimerObserver : FutureObserver<R> {
        CancelTimerObserver(TimerId const& timerId)
            :  m_timerId{timerId}
        {}

        using ValueType = typename FutureObserver<R>::ValueType;

        auto OnFutureReady(ValueType const&) noexcept -> void override {
            m_timerId.Cancel();
        }

        auto OnFutureFail(Status cause) noexcept -> void override {
            if(cause != Status::TIMEOUT) {
                m_timerId.Cancel();
            }
        }

    private:
        TimerId m_timerId;
    };
}

#endif //NANO_CAF_2_42AEF34BED444F53990C7B61B8CC7710
