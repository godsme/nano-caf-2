//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_DEDD159F43C5440295CE21A9860AB502
#define NANO_CAF_2_DEDD159F43C5440295CE21A9860AB502

#include <nano-caf/Status.h>
#include <nano-caf/actor/ActorPtr.h>

namespace nano_caf {
    template<typename T>
    struct Future;

    template<typename T>
    struct AbstractPromiseBase {
        virtual auto OnFail(Status, ActorPtr&& to) noexcept -> void = 0;
        virtual auto Connect(Future<T>&&, ActorWeakPtr&&) noexcept -> void = 0;
        virtual ~AbstractPromiseBase() = default;
    };

    template<typename T>
    struct AbstractPromise : AbstractPromiseBase<T> {
        virtual auto Reply(T&& value, ActorPtr&&) noexcept -> void = 0;
    };

    template<>
    struct AbstractPromise<void> : AbstractPromiseBase<void> {
        virtual auto Reply(ActorPtr&&) noexcept -> void = 0;
    };
}

#endif //NANO_CAF_2_DEDD159F43C5440295CE21A9860AB502
