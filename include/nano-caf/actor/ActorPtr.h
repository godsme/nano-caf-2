//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E
#define NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E

#include <nano-caf/util/SharedPtr.h>
#include <nano-caf/actor/AbstractActor.h>
#include <nano-caf/actor/ExitReason.h>

namespace nano_caf {
    struct Message;

    struct ActorPtr : SharedPtr<AbstractActor> {
        using Parent = SharedPtr<AbstractActor>;
        using Parent::Parent;

        ActorPtr(Parent const& another) : Parent{another} {}
        ActorPtr(Parent&& another) : Parent{std::move(another)} {}

        ActorPtr(ActorPtr const&) = default;
        ActorPtr(ActorPtr&&) = default;

        auto operator=(ActorPtr const&) noexcept -> ActorPtr& = default;
        auto operator=(ActorPtr&&) noexcept -> ActorPtr& = default;

        auto ActorId() const noexcept -> intptr_t {
            return reinterpret_cast<intptr_t>(Parent::m_ptr);
        }

        auto SendMsg(Message*) const noexcept -> Status;
        auto Wait(ExitReason& reason) noexcept -> Status;
    };
}

#endif //NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E
