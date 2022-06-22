//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_C19CAC8598104437AF09481730A40790
#define NANO_CAF_2_C19CAC8598104437AF09481730A40790

#include <nano-caf/Status.h>

namespace nano_caf::detail {
    struct FutureObserver {
        virtual auto OnFutureReady() noexcept -> void = 0;
        virtual auto OnFutureFail(Status cause) noexcept -> void = 0;
        virtual ~FutureObserver() = default;
    };
}

#endif //NANO_CAF_2_C19CAC8598104437AF09481730A40790
