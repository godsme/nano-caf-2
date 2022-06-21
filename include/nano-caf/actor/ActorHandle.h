//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
#define NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2

#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct ActorHandle : private ActorPtr {
        using ActorPtr::ActorPtr;

        using ActorPtr::operator bool;

        ActorHandle(ActorPtr&& rhs) : ActorPtr(std::move(rhs)) {}

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
