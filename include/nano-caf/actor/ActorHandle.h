//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
#define NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2

#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/msg/MakeMessage.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct ActorHandle : private ActorPtr {
        using ActorPtr::ActorPtr;

        using ActorPtr::operator bool;

        ActorHandle(ActorPtr&& rhs) : ActorPtr(std::move(rhs)) {}

        template<typename MSG, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
        auto Send(ARGS&& ... args) const noexcept -> Status {
            return Send(MakeMessage<MSG, CATEGORY>(std::forward<ARGS>(args)...));
        }

        template<typename MSG, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
        auto Send(ActorHandle const& sender, ARGS&& ... args) const noexcept -> Status {
            return Send(MakeMessage<MSG, CATEGORY>(static_cast<ActorPtr const&>(sender), std::forward<ARGS>(args)...));
        }

        template<typename MSG, Message::Category CATEGORY = Message::NORMAL, typename HANDLER, typename ... ARGS>
        auto Request(ActorHandle const& sender, HANDLER&& handler, ARGS&& ... args) const noexcept -> Status {
            return Send(MakeRequest<MSG, CATEGORY>(static_cast<ActorPtr const&>(sender), std::forward<HANDLER>(handler), std::forward<ARGS>(args)...));
        }

        auto Wait(ExitReason& reason) noexcept -> Status;

    private:
        auto Send(Message*) const noexcept -> Status;
    };
}

#endif //NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
