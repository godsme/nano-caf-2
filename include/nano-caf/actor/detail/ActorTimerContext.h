//
// Created by Darwin Yuan on 2022/7/17.
//

#ifndef NANO_CAF_2_E31D54CB6C2A4111BF9929487F8CC06A
#define NANO_CAF_2_E31D54CB6C2A4111BF9929487F8CC06A

#include <nano-caf/timer/TimerSpec.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/actor/detail/ExpectOnceMsgHandlers.h>

namespace nano_caf::detail {
    struct ActorTimerContext : private ExpectOnceMsgHandlers {
        auto StartTimer(ActorHandle&& self, TimerSpec const& spec, std::size_t repeatTimes, TimeoutCallback&&) noexcept -> nano_caf::Result<TimerId>;
        auto StartExpectMsgTimer(ActorHandle&& self, TimerSpec const& spec, std::shared_ptr<detail::CancellableMsgHandler> const&) noexcept -> Result<TimerId>;
        auto StartFutureTimer(ActorHandle&& self, TimerSpec const& spec, std::shared_ptr<PromiseDoneNotifier> const&) noexcept -> Result<TimerId>;
        auto ClearAllTimers(intptr_t self) -> void;

        using ExpectOnceMsgHandlers::AddHandler;
        using ExpectOnceMsgHandlers::HandleMsg;

    private:
        bool timerUsed{false};
    };
}

#endif //NANO_CAF_2_E31D54CB6C2A4111BF9929487F8CC06A
