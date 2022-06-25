//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A
#define NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A

#include <nano-caf/scheduler/Coordinator.h>
#include <nano-caf/Status.h>
#include <nano-caf/timer/ActorTimer.h>

namespace nano_caf {
    struct Resumable;

    struct ActorSystem : Coordinator, private ActorTimer {
        static auto Instance() noexcept -> ActorSystem&;

        using ActorTimer::StartTimer;
        using ActorTimer::StopTimer;
        using ActorTimer::ClearActorTimer;

        using Coordinator::Schedule;

    private:
        ActorSystem() = default;
    };
}

#endif //NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A
