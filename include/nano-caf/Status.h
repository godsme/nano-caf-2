//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_6021331FF3284E0FBAD637054B062B4D
#define NANO_CAF_2_6021331FF3284E0FBAD637054B062B4D

#include <stdint.h>

namespace nano_caf {
    enum class Status : uint32_t {
        OK,
        FAILED,
        TIMEOUT,
        OUT_OF_MEM,
        NULL_PTR,
        NULL_ACTOR,
        CLOSED,
        DISCARDED,
        SHUTDOWN,
        BLOCKED
    };
}

#endif //NANO_CAF_2_6021331FF3284E0FBAD637054B062B4D
