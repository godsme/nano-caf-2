//
// Created by Darwin Yuan on 2022/8/20.
//

#ifndef NANO_CAF_2_E516833D8D28454B8EED85333CC226DE
#define NANO_CAF_2_E516833D8D28454B8EED85333CC226DE

#include <nano-caf/util/WeakPtr.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/actor/AbstractActor.h>

namespace nano_caf {
    struct ActorWeakPtr : WeakPtr<AbstractActor> {
        using Parent = WeakPtr<AbstractActor>;
        using Parent::Parent;

        ActorWeakPtr(ActorWeakPtr const&) = default;
        ActorWeakPtr(ActorWeakPtr&&) = default;

        auto operator=(ActorWeakPtr const&) noexcept -> ActorWeakPtr& = default;
        auto operator=(ActorWeakPtr&&) noexcept -> ActorWeakPtr& = default;

        auto Wait(ExitReason& reason) -> Status;
    };
}

#endif //NANO_CAF_2_E516833D8D28454B8EED85333CC226DE
