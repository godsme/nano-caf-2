//
// Created by Darwin Yuan on 2022/6/25.
//
#include <nano-caf/timer/TimerSet.h>
#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/util/ShutdownNotifier.h>

namespace nano_caf {
    namespace {
        inline auto GetDue(StartTimerMsg const* msg) {
            return msg->spec.LeftMatch([&](auto const& duration) {
                return msg->issue_time_point + std::chrono::microseconds(duration);
            });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::AddTimer(std::unique_ptr<Message> msg) -> Status {
        if(msg == nullptr) return Status::NULL_PTR;

        auto startMsg = msg->Body<StartTimerMsg>();
        if(startMsg == nullptr) return Status::NULL_PTR;

        auto due = GetDue(startMsg);
        while(due < std::chrono::steady_clock::now()) {
            if(startMsg->callback() != Status::OK) {
                return Status::OK;
            }

            if(!startMsg->is_periodic) {
                return Status::OK;
            } else {
                startMsg->issue_time_point = due;
                due = GetDue(startMsg);
            }
        }

        auto iterator = m_timers.emplace(due, std::move(msg));
        m_actorIndexer.emplace(startMsg->actor_id, iterator);

        return Status::OK;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    template<typename PRED, typename OP>
    auto TimerSet::TimerFindAndModify(int code, intptr_t actor_id, PRED&& pred, OP&& op) -> void {
        auto range = m_actorIndexer.equal_range(actor_id);
        auto result = std::find_if(range.first, range.second, std::forward<PRED>(pred));
        if(result != range.second) {
            op(result);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::RemoveTimer(intptr_t actor_id, TimerId timer_id) -> void {
            TimerFindAndModify(0, actor_id,
                              [&](auto const& item) {
                                  auto&& [_, msg] = *item.second;
                                  return msg->template Body<StartTimerMsg>()->id == timer_id;
                              },
                              [&](auto const& result) {
                                  m_timers.erase(result->second);
                                  m_actorIndexer.erase(result);
                              });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::RemoveIndex(intptr_t actor_id, Timers ::iterator const& iterator) -> void {
        TimerFindAndModify(1, actor_id,
                          [&](auto const& item)      { return item.second == iterator; },
                          [this](auto const& result) { m_actorIndexer.erase(result);
                          });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::UpdateIndex(intptr_t actor_id, Timers::iterator const& from, Timers::iterator const& to) -> void {
        TimerFindAndModify(2, actor_id,
                          [&](auto const& item) { return item.second == from; },
                          [&](auto& result)     { result->second = to; });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::ClearActorTimers(intptr_t actorId) -> void {
        auto range = m_actorIndexer.equal_range(actorId);
        std::for_each(range.first, range.second, [this](auto const& elem){
            m_timers.erase(elem.second);
        });
        m_actorIndexer.erase(range.first, range.second);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::HandleMsg(std::unique_ptr<Message> msg) -> void {
        switch(msg->id) {
            case StartTimerMsg::ID:
                AddTimer(std::move(msg));
                break;
            case StopTimerMsg::ID: {
                auto stop_msg = msg->Body<StopTimerMsg>();
                RemoveTimer(stop_msg->actor, stop_msg->id);
                break;
            }
            case ClearActorTimerMsg::ID: {
                auto clear_msg = msg->Body<ClearActorTimerMsg>();
                ClearActorTimers(clear_msg->actor);
                break;
            }
            default: break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    namespace {
        inline auto CleanupMsgs(Message* msgs) {
            while(msgs != nullptr) {
                std::unique_ptr<Message> head{msgs};
                msgs = head->m_next;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::HandleMsgs(Message *msgs, ShutdownNotifier const& shutdown) noexcept -> Status {
        while(msgs != nullptr) {
            if(CheckTimerDue(shutdown) == Status::SHUTDOWN) {
                CleanupMsgs(msgs);
                return Status::SHUTDOWN;
            }

            std::unique_ptr<Message> head{msgs};
            msgs = head->m_next;
            head->m_next = nullptr;

            HandleMsg(std::move(head));
        }
        return Status::OK;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::CheckTimerDue(ShutdownNotifier const& shutdown) noexcept -> Status {
        while(!m_timers.empty()) {
            if(shutdown.ShutdownNotified()) {
                return Status::SHUTDOWN;
            }

            auto timer_iter = m_timers.begin();
            auto&& [due, msg] = *timer_iter;

            if(due > std::chrono::steady_clock::now()) {
                break;
            }

            auto timerMsg = msg->Body<StartTimerMsg>();

            if(timerMsg->callback() != Status::OK || !timerMsg->is_periodic) {
                RemoveIndex(timerMsg->actor_id, timer_iter);
                m_timers.erase(timer_iter);
            } else {
                timerMsg->issue_time_point = due;
                auto sched_msg = std::move(timer_iter->second);

                m_timers.erase(timer_iter);
                auto iterator = m_timers.emplace(GetDue(timerMsg), std::move(sched_msg));
                UpdateIndex(timerMsg->actor_id, timer_iter, iterator);
            }
        }

        return Status::OK;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto TimerSet::Reset() noexcept -> void {
        m_actorIndexer.clear();
        m_timers.clear();
    }
}