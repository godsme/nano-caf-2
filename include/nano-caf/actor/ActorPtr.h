//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E
#define NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E

#include <nano-caf/util/WeakPtr.h>
#include <nano-caf/actor/AbstractActor.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>
#include <nano-caf/util/Assertions.h>

namespace nano_caf {
    using ActorPtr = SharedPtr<AbstractActor>;
    struct ActorWeakPtr : WeakPtr<AbstractActor> {
        using Parent = WeakPtr<AbstractActor>;
        using Parent::Parent;

        ActorWeakPtr(ActorWeakPtr const&) = default;
        ActorWeakPtr(ActorWeakPtr&&) = default;

        auto operator=(ActorWeakPtr const&) noexcept -> ActorWeakPtr& = default;
        auto operator=(ActorWeakPtr&&) noexcept -> ActorWeakPtr& = default;

        auto Wait(ExitReason& reason) -> Status {
            auto ctlBlock = reinterpret_cast<detail::ActorCtlBlock*>(Parent::CtlBlock());
            CAF_ASSERT_VALID_PTR(ctlBlock);
            return ctlBlock->Wait(reason);
        }
    };
}

#endif //NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E
