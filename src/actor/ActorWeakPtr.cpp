//
// Created by Darwin Yuan on 2022/8/20.
//
#include <nano-caf/actor/ActorWeakPtr.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>
#include <nano-caf/util/Assertions.h>

namespace nano_caf {
    auto ActorWeakPtr::Wait(ExitReason& reason) -> Status {
        auto ctlBlock = reinterpret_cast<detail::ActorCtlBlock*>(Parent::CtlBlock());
        CAF_ASSERT(ctlBlock);
        return ctlBlock->Wait(reason);
    }

    auto ActorWeakPtr::Lock() const noexcept -> ActorPtr {
        return ActorPtr{Parent::Lock()};
    }
}