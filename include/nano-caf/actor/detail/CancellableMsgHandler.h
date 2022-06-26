//
// Created by Darwin Yuan on 2022/6/26.
//

#ifndef NANO_CAF_2_DE4A107045464D00A9DD97D6D5DD00B7
#define NANO_CAF_2_DE4A107045464D00A9DD97D6D5DD00B7

#include <nano-caf/actor/detail/MsgHandler.h>

namespace nano_caf::detail {
    struct CancellableMsgHandler : MsgHandler {
        virtual auto Cancel() noexcept -> void = 0;
    };
}

#endif //NANO_CAF_2_DE4A107045464D00A9DD97D6D5DD00B7
