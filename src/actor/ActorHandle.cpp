//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/actor/ActorHandle.h>

namespace nano_caf {
    auto ActorHandle::Send(Message* msg) const noexcept -> Status {
        auto actor = ActorPtr::Get();
        if(actor == nullptr) return Status::NULL_PTR;

        switch (actor->SendMsg(msg)) {
            case LifoQueue::Result::OK:
                return Status::OK;
            case LifoQueue::Result::CLOSED:
                delete msg;
                return Status::CLOSED;
            case LifoQueue::Result::BLOCKED: {
                // TODO:
                return Status::OK;
            }
            default:
                return Status::NULL_PTR;
        }
    }
}