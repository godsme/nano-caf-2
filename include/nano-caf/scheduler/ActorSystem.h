//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A
#define NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/timer/TimerSpec.h>
#include <nano-caf/timer/TimerId.h>
#include <nano-caf/timer/TimeoutCallback.h>
#include <nano-caf/util/Result.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct Resumable;

    struct ActorSystem {
        static auto Instance() noexcept -> ActorSystem&;

        auto StartUp(std::size_t numOfWorkers) noexcept -> Status;
        auto Shutdown() noexcept -> void;

        auto StartTimer( ActorHandle const& self,
                         TimerSpec const& spec,
                         std::size_t repeatTimes,
                         TimeoutCallback&& callback) -> Result<TimerId>;

        auto StopTimer(TimerId const&) -> Status;
        auto ClearActorTimer(intptr_t actorId) -> Status;

        auto Schedule(Resumable*) noexcept -> Status;

    private:
        ActorSystem() = default;

    private:
        struct ActorSystemImpl;
        ActorSystemImpl *m_impl;
    };
}

#endif //NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A
