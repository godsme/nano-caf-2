//
// Created by Darwin Yuan on 2022/7/17.
//

#include <nano-caf/actor/detail/ActorTimerContext.h>
#include <nano-caf/msg/PredefinedMsgs.h>

namespace nano_caf::detail {
    auto ActorTimerContext::StartTimer(ActorHandle&& self, TimerSpec const& spec, std::size_t repeatTimes, TimeoutCallback&& callback) noexcept -> Result<TimerId> {
        auto result = ActorSystem::Instance().StartTimer(self, spec, repeatTimes, std::move(callback));
        if(result.Ok()) {
            timerUsed = true;
        }
        return result;
    }

    auto ActorTimerContext::StartExpectMsgTimer(ActorHandle&& self, TimerSpec const& spec, std::shared_ptr<detail::CancellableMsgHandler> const& handler) noexcept -> Result<TimerId> {
        using WeakType = std::weak_ptr<detail::CancellableMsgHandler>;
        return StartTimer(std::move(self), spec, 1,
                [this, weak = WeakType{handler}](ActorHandle actor, TimerId const &timerId) mutable -> Status {
                    auto &&handler = weak.lock();
                    if (!handler || !handler->OnTimeout()) return Status::FAILED;
                    return actor.Send<TimeoutMsg>(timerId,
                        [this, weak = std::move(weak)] {
                                auto &&handler = weak.lock();
                                if (handler){
                                    handler->Cancel();
                                    ExpectOnceMsgHandlers::RemoveHandler(handler);
                                }
                        });
                });
        }

    auto ActorTimerContext::StartFutureTimer(ActorHandle&& self, TimerSpec const& spec, std::shared_ptr<PromiseDoneNotifier> const& notifier) noexcept -> Result<TimerId> {
        return StartTimer(std::move(self), spec, 1,
                    [weakNotifier = std::weak_ptr<PromiseDoneNotifier>{notifier}](ActorHandle& actor, TimerId const&) mutable -> Status {
                        auto&& future = weakNotifier.lock();
                        if(!future || !future->OnTimeout()) return Status::FAILED;
                        return actor.Send<FutureDoneNotify>(std::move(future));
                });
    }

    auto ActorTimerContext::ClearAllTimers(intptr_t self) -> void {
        if(timerUsed) {
            ActorSystem::Instance().ClearActorTimer(self);
        }
    }
}