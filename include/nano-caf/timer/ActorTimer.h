//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_CB3152A5CFC64E9EAEC4894C2E89EC49
#define NANO_CAF_2_CB3152A5CFC64E9EAEC4894C2E89EC49

#include <nano-caf/actor/LifoQueue.h>
#include <nano-caf/timer/TimerSet.h>
#include <nano-caf/timer/TimerSpec.h>
#include <nano-caf/timer/TimeoutCallback.h>
#include <nano-caf/util/CvNotifier.h>
#include <nano-caf/util/Result.h>
#include <nano-caf/util/ShutdownNotifier.h>
#include <thread>

namespace nano_caf {
    struct ActorHandle;

    struct ActorTimer {
        auto Start() -> void;
        auto Stop() -> void;

        auto StartTimer( ActorHandle const& self,
                          TimerSpec const& spec,
                          bool periodic,
                         TimeoutCallback&& callback) -> Result<TimerId>;

        auto StopTimer(ActorHandle const& self, TimerId) -> Status;
        auto ClearActorTimer(ActorHandle const& self) -> Status;

    private:
        auto Schedule() -> void;
        auto GoSleep() -> Status;
        auto SendMsg(Message*) -> Status;
        auto TryGoSleep() -> Status;
        auto Process(Message* msgs) -> Status;

    private:
        std::thread m_thread;
        TimerSet m_timers{};

        LifoQueue msg_queue_{};
        CvNotifier cv_{};

        ShutdownNotifier shutdown_{};

        std::atomic<uint64_t> timer_id_{0};
    };
}

#endif //NANO_CAF_2_CB3152A5CFC64E9EAEC4894C2E89EC49
