//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_958CE759B2D04EB88F7CD831EF002D9B
#define NANO_CAF_2_958CE759B2D04EB88F7CD831EF002D9B

#include <nano-caf/scheduler/list_elem.h>

namespace nano_caf {
    struct resumable : list_elem<resumable> {
        virtual ~resumable() = default;

        virtual auto resume() noexcept -> bool = 0;

        virtual auto add_ref() noexcept -> void {}
        virtual auto release() noexcept -> void {
            delete this;
        }
    };
}

#endif //NANO_CAF_2_958CE759B2D04EB88F7CD831EF002D9B
