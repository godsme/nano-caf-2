//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_191F08152DC9427FAF4279A54F16930F
#define NANO_CAF_2_191F08152DC9427FAF4279A54F16930F

#include <nano-caf/Status.h>
#include <functional>

namespace nano_caf {
    using FailHandler = std::function<auto (Status) -> void>;
}

#endif //NANO_CAF_2_191F08152DC9427FAF4279A54F16930F
