//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_C065636D56EF4329AE2EF93354B0F81E
#define NANO_CAF_2_C065636D56EF4329AE2EF93354B0F81E

#include <nano-caf/scheduler/ListElem.h>

namespace nano_caf {
    struct Resumable : ListElem<Resumable> {
        virtual ~Resumable() = default;

        virtual auto Resume() noexcept -> void = 0;

        virtual auto AddRef() noexcept -> void {}
        virtual auto Release() noexcept -> void {
            delete this;
        }
    };
}

#endif //NANO_CAF_2_C065636D56EF4329AE2EF93354B0F81E
