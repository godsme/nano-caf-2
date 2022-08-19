//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/util/Assertions.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>

namespace nano_caf {
    auto ActorHandle::Wait(ExitReason& reason) noexcept -> Status {
        auto ctrBlock = reinterpret_cast<detail::ActorCtlBlock*>(ActorPtr::CtlBlock());
        CAF_ASSERT_VALID_PTR_R(ctrBlock, Status::NULL_ACTOR);
        auto result = ctrBlock->Wait(reason);
        Release();
        return result;
    }

    auto ActorHandle::SendMsg(Message* msg) const noexcept -> Status {
        auto actor = ActorPtr::Get();
        CAF_ASSERT_VALID_PTR_R(actor, Status::NULL_ACTOR);
        return actor->Send(msg);
    }

    auto ActorHandle::Exit(ExitReason reason) noexcept -> Status {
        auto actor = ActorPtr::Get();
        CAF_ASSERT_VALID_PTR_R(actor, Status::NULL_ACTOR);
        Send<ExitMsg, Message::URGENT>(reason);
        Release();
        return Status::OK;
    }
}