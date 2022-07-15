//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_166EF8EB5ED44AD28868C99697524FFA
#define NANO_CAF_2_166EF8EB5ED44AD28868C99697524FFA

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

    struct ActorTimerSystem {
        ActorTimerSystem() = default;

        auto Start() -> void;
        auto Stop() -> void;

        auto StartTimer( ActorHandle const& self,
                         TimerSpec const& spec,
                         bool periodic,
                         TimeoutCallback&& callback) -> Result<TimerId>;

        auto StopTimer(intptr_t self, TimerId const&) -> Status;
        auto ClearActorTimer(intptr_t self) -> Status;

    private:
        auto Schedule() -> void;
        auto GoSleep() -> Status;
        auto SendMsg(Message*) -> Status;
        auto TryGoSleep() -> Status;
        auto Process(Message* msgs) -> Status;

    private:
        std::thread m_thread;
        TimerSet m_timers{};

        LifoQueue m_queue{};
        CvNotifier m_cv{};

        ShutdownNotifier m_shutdown{};

        std::atomic<uint64_t> m_timerId{0};
        bool m_working{false};
    };
}

#endif //NANO_CAF_2_166EF8EB5ED44AD28868C99697524FFA
