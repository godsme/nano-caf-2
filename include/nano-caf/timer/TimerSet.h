//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_834640CA524F4862A6A6B787C63535C1
#define NANO_CAF_2_834640CA524F4862A6A6B787C63535C1

#include <nano-caf/actor/LifoQueue.h>
#include <nano-caf/Status.h>
#include <nano-caf/timer/TimerId.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/util/ShutdownNotifier.h>
#include <nano-caf/msg/Message.h>
#include <map>

namespace nano_caf {
    struct TimerSet {
        TimerSet();

    private:
        using TimePoint = std::chrono::steady_clock::time_point;

    public:
        auto HandleMsgs(Message* msgs, ShutdownNotifier const &) noexcept -> Status;
        auto Reset() noexcept -> void;
        auto Empty() const noexcept -> bool {
            return m_timers.empty();
        }

        auto GetRecentDue() -> TimePoint {
            return m_timers.begin()->first;
        }

        auto CheckTimerDue(ShutdownNotifier const&) noexcept -> Status;

    private:
        auto AddTimer(std::unique_ptr<Message> msg) -> Status;
        auto RemoveTimer(intptr_t, TimerId) -> void;
        auto ClearActorTimers(intptr_t) -> void;

        auto HandleMsg(std::unique_ptr<Message> msg) -> void;

        template<typename PRED, typename OP>
        auto TimerFindAndModify(int code, intptr_t actor_id, PRED&& pred, OP&& op) -> void;

    private:
        using Timers = std::multimap<TimePoint, std::unique_ptr<Message>>;

        auto RemoveIndex(intptr_t, Timers::iterator const&) -> void;
        auto UpdateIndex(intptr_t, Timers::iterator const&, Timers::iterator const&) -> void;

    private:
        Timers m_timers{};
        std::multimap<intptr_t, Timers::iterator> m_actorIndexer{};
    };
}

#endif //NANO_CAF_2_834640CA524F4862A6A6B787C63535C1
