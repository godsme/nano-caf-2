//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_C19CAC8598104437AF09481730A40790
#define NANO_CAF_2_C19CAC8598104437AF09481730A40790

#include <nano-caf/Status.h>
#include <nano-caf/util/Void.h>

namespace nano_caf::detail {
    template<typename R>
    struct FutureObserver {
        using ValueType = typename ValueTypeTrait<R>::ValueType;
        virtual auto OnFutureReady(ValueType const&) noexcept -> void = 0;
        virtual auto OnFutureFail(Status cause) noexcept -> void = 0;
        virtual ~FutureObserver() = default;
    };
}

#endif //NANO_CAF_2_C19CAC8598104437AF09481730A40790
