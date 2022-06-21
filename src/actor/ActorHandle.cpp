//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf {
    auto ActorHandle::Wait(ExitReason& reason) noexcept -> Status {
        auto actor = ActorPtr::Get();
        if(actor == nullptr) return Status::NULL_ACTOR;
        return actor->Wait(reason);
    }

    auto ActorHandle::Send(Message* msg) const noexcept -> Status {
        auto actor = ActorPtr::Get();
        if(actor == nullptr) return Status::NULL_ACTOR;

        switch (actor->SendMsg(msg)) {
            case LifoQueue::Result::OK:
                return Status::OK;
            case LifoQueue::Result::CLOSED:
                return Status::CLOSED;
            case LifoQueue::Result::BLOCKED: {
                return ActorSystem::Instance().Schedule(actor);
            }
            default:
                return Status::NULL_PTR;
        }
    }
}