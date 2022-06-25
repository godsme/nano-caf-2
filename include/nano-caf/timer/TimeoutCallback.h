//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_99A9E6548D0F42F28097BD1F64AD24BC
#define NANO_CAF_2_99A9E6548D0F42F28097BD1F64AD24BC

#include <nano-caf/timer/TimerId.h>
#include <functional>

namespace nano_caf {
    using TimeoutCallback = std::function<auto (TimerId) -> void>;
}

#endif //NANO_CAF_2_99A9E6548D0F42F28097BD1F64AD24BC
