//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf {
    auto ActorHandle::Wait(ExitReason& reason) noexcept -> Status {
        auto actor = ActorPtr::Get();
        if(actor == nullptr) return Status::NULL_ACTOR;
        auto result = actor->Wait(reason);
        Release();
        return result;
    }

    auto ActorHandle::Send(Message* msg) const noexcept -> Status {
        auto actor = ActorPtr::Get();
        if(actor == nullptr) return Status::NULL_ACTOR;
        return actor->SendMsg(msg);
    }
}