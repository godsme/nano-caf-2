//
// Created by Darwin Yuan on 2022/8/20.
//

#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/util/Assertions.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>

namespace nano_caf {
    auto ActorPtr::Wait(ExitReason& reason) noexcept -> Status {
        auto ctrBlock = reinterpret_cast<detail::ActorCtlBlock*>(Parent::CtlBlock());
        CAF_ASSERT_VALID_PTR_R(ctrBlock, Status::NULL_ACTOR);
        auto result = ctrBlock->Wait(reason);
        Release();
        return result;
    }

    auto ActorPtr::SendMsg(Message* msg) const noexcept -> Status {
        auto actor = Parent::Get();
        CAF_ASSERT_VALID_PTR_R(actor, Status::NULL_ACTOR);
        return actor->Send(msg);
    }
}