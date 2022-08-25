//
// Created by Darwin Yuan on 2022/7/17.
//

#ifndef NANO_CAF_2_E31D54CB6C2A4111BF9929487F8CC06A
#define NANO_CAF_2_E31D54CB6C2A4111BF9929487F8CC06A

#include <nano-caf/timer/TimerSpec.h>
#include <nano-caf/actor/detail/ExpectOnceMsgHandlers.h>
#include <nano-caf/async/PromiseDoneNotifier.h>
#include <nano-caf/timer/TimeoutCallback.h>
#include <nano-caf/util/Result.h>

namespace nano_caf::detail {
    struct ActorTimerContext : private ExpectOnceMsgHandlers {
        auto StartTimer(ActorPtr&& self, TimerSpec const& spec, std::size_t repeatTimes, TimeoutCallback&&) const noexcept -> Result<TimerId>;
        auto StartExpectMsgTimer(ActorPtr&& self, TimerSpec const& spec, std::shared_ptr<detail::CancellableMsgHandler> const&) noexcept -> Result<TimerId>;
        auto StartFutureTimer(ActorPtr&& self, TimerSpec const& spec, std::shared_ptr<PromiseDoneNotifier> const&) const noexcept -> Result<TimerId>;
        auto ClearAllTimers(intptr_t self) -> void;

        using ExpectOnceMsgHandlers::AddHandler;
        using ExpectOnceMsgHandlers::HandleMsg;

    private:
        mutable bool timerUsed{false};
    };
}

#endif //NANO_CAF_2_E31D54CB6C2A4111BF9929487F8CC06A
