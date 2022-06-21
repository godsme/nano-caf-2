//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A
#define NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A

#include <nano-caf/scheduler/Coordinator.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct Resumable;

    struct ActorSystem : Coordinator {
        static auto Instance() noexcept -> ActorSystem&;

    private:
        ActorSystem() = default;
    };
}

#endif //NANO_CAF_2_EFCBBF89BBB743A7AB631E7E4464AC6A
