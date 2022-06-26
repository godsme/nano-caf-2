//
// Created by Darwin Yuan on 2022/6/25.
//
#include <nano-caf/timer/ActorTimerSystem.h>
#include <nano-caf/msg/MakeMessage.h>
#include <nano-caf/actor/ActorHandle.h>

namespace nano_caf {

    /////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::GoSleep() -> Status {
        auto pred = [this] {
            return m_shutdown.ShutdownNotified() || !m_queue.IsBlocked();
        };

        if(m_timers.Empty()) {
            m_cv.Wait(pred);
        } else {
            if (!m_cv.WaitUntil(m_timers.GetRecentDue(), pred)) {
                return m_timers.CheckTimerDue(m_shutdown);
            }
        }

        return m_shutdown.ShutdownNotified() ? Status::SHUTDOWN : Status::OK;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    inline auto ActorTimerSystem::TryGoSleep() -> Status {
        if(m_queue.TryBlock()) return GoSleep();
        return Status::OK;
    }

    namespace {
        auto Reorder(Message* msgs) noexcept -> Message* {
            Queue<Message> queue{};
            while (msgs != nullptr) {
                auto ptr = msgs;
                msgs = msgs->m_next;
                queue.PushFront(ptr);
            }
            return queue.TakeAll();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::Process(Message* msgs) -> Status {
        if(msgs == nullptr) return TryGoSleep();
        if(msgs->m_next != nullptr) {
            msgs = Reorder(msgs);
        }

        return m_timers.HandleMsgs(msgs, m_shutdown);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::Schedule() -> void {
        while(Process(m_queue.TakeAll()) != Status::SHUTDOWN);
        m_queue.Close();
        m_timers.Reset();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::Start() -> void {
        if(m_working) return;
        m_thread = std::thread([this]{ Schedule(); });
        m_working = true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::Stop() -> void {
        if(!m_working) return;
        m_shutdown.NotifyShutdown();
        m_cv.WakeUp();
        m_thread.join();
        m_working = false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::SendMsg(Message* msg) -> Status {
        switch(m_queue.Enqueue(msg)) {
            case LifoQueue::Result::OK: return Status::OK;
            case LifoQueue::Result::BLOCKED: {
                m_cv.WakeUp();
                return Status::OK;
            }
            case LifoQueue::Result::NULL_MSG: return Status::NULL_PTR;
            case LifoQueue::Result::CLOSED:   return Status::CLOSED;
            default: return Status::FAILED;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::StartTimer
            ( ActorHandle const& sender
                    , TimerSpec const& spec
                    , bool periodic
                    , TimeoutCallback && callback) -> Result<TimerId> {
        if(!m_working) { return Status::CLOSED; }
        if(!sender) { return Status::NULL_ACTOR; }

        TimerId id{m_timerId.fetch_add(1, std::memory_order_relaxed)};
        auto status = SendMsg(MakeMessage<StartTimerMsg>(
                id, std::move(sender.ToWeakPtr()), spec, std::chrono::steady_clock::now(), periodic, std::move(callback)));
        if(status != Status::OK) return status;
        return id;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::StopTimer(ActorHandle const& self, TimerId id) -> Status {
        if(!m_working) { return Status::CLOSED; }
        if(!self) { return Status::NULL_ACTOR; }
        return SendMsg(MakeMessage<StopTimerMsg>(self.ActorId(), id));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto ActorTimerSystem::ClearActorTimer(ActorHandle const& self) -> Status {
        if(!m_working) { return Status::CLOSED; }
        if(!self) { return Status::NULL_ACTOR; }
        return SendMsg(MakeMessage<ClearActorTimerMsg>(self.ActorId()));
    }
}