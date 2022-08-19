//
// Created by Darwin Yuan on 2022/8/20.
//

#ifndef NANO_CAF_2_E516833D8D28454B8EED85333CC226DE
#define NANO_CAF_2_E516833D8D28454B8EED85333CC226DE

#include <nano-caf/util/WeakPtr.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/actor/AbstractActor.h>
#include <nano-caf/actor/ActorPtr.h>

namespace nano_caf {
    struct ActorWeakPtr : private WeakPtr<AbstractActor> {
        using Parent = WeakPtr<AbstractActor>;
        using Parent::Parent;

        ActorWeakPtr(ActorPtr const& p) : Parent{p.Get()} {}
        ActorWeakPtr(ActorPtr&& p) : Parent{p.Get()} {}

        ActorWeakPtr(ActorWeakPtr const&) = default;
        ActorWeakPtr(ActorWeakPtr&&) = default;

        auto operator=(ActorWeakPtr const&) noexcept -> ActorWeakPtr& = default;
        auto operator=(ActorWeakPtr&&) noexcept -> ActorWeakPtr& = default;

        using Parent::UseCount;
        using Parent::Reset;
        using Parent::Expired;

        auto Wait(ExitReason& reason) -> Status;
        auto Lock() const noexcept -> ActorPtr;
    };
}

#endif //NANO_CAF_2_E516833D8D28454B8EED85333CC226DE
