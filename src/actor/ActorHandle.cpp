//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/util/Assertions.h>

namespace nano_caf {
    auto ActorHandle::Wait(ExitReason& reason) noexcept -> Status {
        auto actor = ActorPtr::Get();
        CAF_ASSERT_VALID_PTR_R(actor, Status::NULL_ACTOR);
        auto result = actor->Wait(reason);
        Release();
        return result;
    }

    auto ActorHandle::SendMsg(Message* msg) const noexcept -> Status {
        auto actor = ActorPtr::Get();
        CAF_ASSERT_VALID_PTR_R(actor, Status::NULL_ACTOR);
        return actor->SendMsg(msg);
    }

    auto ActorHandle::Exit(ExitReason reason) noexcept -> Status {
        auto actor = ActorPtr::Get();
        CAF_ASSERT_VALID_PTR_R(actor, Status::NULL_ACTOR);
        Send<ExitMsg, Message::URGENT>(reason);
        Release();
        return Status::OK;
    }
}