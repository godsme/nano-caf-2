//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_A4742C7D5BE047FF8D156142EC0061EC
#define NANO_CAF_2_A4742C7D5BE047FF8D156142EC0061EC

#include <nano-caf/util/WeakPtr.h>
#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/actor/ActorHandle.h>

namespace nano_caf {
    struct ActorWeakHandle : WeakPtr<SchedActor> {
        using Parent = WeakPtr<SchedActor>;
        using Parent::Parent;

        auto Lock() const noexcept -> ActorHandle {

        }
    };

}

#endif //NANO_CAF_2_A4742C7D5BE047FF8D156142EC0061EC
