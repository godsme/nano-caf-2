//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_DEDD159F43C5440295CE21A9860AB502
#define NANO_CAF_2_DEDD159F43C5440295CE21A9860AB502

#include <nano-caf/Status.h>
#include <nano-caf/actor/ActorWeakPtr.h>
#include <nano-caf/util/Void.h>

namespace nano_caf {
    template<typename T>
    struct Future;

    struct FailNotifier {
        virtual auto OnFail(Status, ActorWeakPtr&) noexcept -> void = 0;
    };

    template<typename R>
    struct AbstractPromise : FailNotifier {
        virtual auto Reply(ValueTypeOf<R> const&, ActorWeakPtr&) noexcept -> void = 0;
        virtual auto Reply(ValueTypeOf<R>&&, ActorWeakPtr&) noexcept -> void = 0;
        virtual auto Join(Future<R>&&, ActorWeakPtr&) noexcept -> void = 0;
    };
}

#endif //NANO_CAF_2_DEDD159F43C5440295CE21A9860AB502
