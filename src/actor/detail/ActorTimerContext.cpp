//
// Created by Darwin Yuan on 2022/7/17.
//

#include <nano-caf/actor/detail/ActorTimerContext.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/GlobalActorContext.h>
#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf::detail {
    auto ActorTimerContext::StartTimer(ActorPtr&& self, TimerSpec const& spec, std::size_t repeatTimes, TimeoutCallback&& callback) noexcept -> Result<TimerId> {
        auto result = ActorSystem::Instance().StartTimer(self, spec, repeatTimes, std::move(callback));
        if(result.Ok()) {
            timerUsed = true;
        }
        return result;
    }

    auto ActorTimerContext::StartExpectMsgTimer(ActorPtr&& self, TimerSpec const& spec, std::shared_ptr<detail::CancellableMsgHandler> const& handler) noexcept -> Result<TimerId> {
        using WeakType = std::weak_ptr<detail::CancellableMsgHandler>;
        return StartTimer(std::move(self), spec, 1,
                [this, weak = WeakType{handler}](ActorPtr actor, TimerId const &timerId) mutable -> Status {
                    auto &&handler = weak.lock();
                    if (!handler || !handler->OnTimeout()) return Status::FAILED;
                    return GlobalActorContext::Send<TimeoutMsg>(actor, timerId,
                        [this, weak = std::move(weak)] {
                                auto &&handler = weak.lock();
                                if (handler){
                                    handler->Cancel();
                                    ExpectOnceMsgHandlers::RemoveHandler(handler);
                                }
                        });
                });
        }

    auto ActorTimerContext::StartFutureTimer(ActorPtr&& self, TimerSpec const& spec, std::shared_ptr<PromiseDoneNotifier> const& notifier) noexcept -> Result<TimerId> {
        return StartTimer(std::move(self), spec, 1,
                    [weakNotifier = std::weak_ptr<PromiseDoneNotifier>{notifier}](ActorPtr& actor, TimerId const&) mutable -> Status {
                        auto&& future = weakNotifier.lock();
                        if(!future || !future->OnTimeout()) return Status::FAILED;
                        return GlobalActorContext::Send<FutureDoneNotify>(actor, std::move(future));
                });
    }

    auto ActorTimerContext::ClearAllTimers(intptr_t self) -> void {
        if(timerUsed) {
            ActorSystem::Instance().ClearActorTimer(self);
        }
    }
}