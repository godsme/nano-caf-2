//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
#define NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2

#include <nano-caf/util/SharedPtr.h>
#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/message/Message.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct ActorHandle : SharedPtr<SchedActor> {
        using Parent = SharedPtr<SchedActor>;
        using Parent::Parent;

        ActorHandle(SharedPtr<SchedActor>&& rhs) : Parent(std::move(rhs)) {}

        template<typename MSG, Message::Category category = Message::NORMAL, typename ... ARGS>
        auto Send(ARGS&& ... args) const noexcept -> Status {
            return Status::FAILED;
        }

        template<typename MSG, Message::Category category = Message::NORMAL, typename ... ARGS>
        auto Send(ActorHandle const& sender, ARGS&& ... args) const noexcept -> Status {
            return Status::FAILED;
        }

    private:
        auto Send(Message*) const noexcept -> Status;
    };
}

#endif //NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
