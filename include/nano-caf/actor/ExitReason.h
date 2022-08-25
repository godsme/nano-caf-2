//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_190579E26BF84FB688E1A5A2704684E6
#define NANO_CAF_2_190579E26BF84FB688E1A5A2704684E6

#include <cstdint>

namespace nano_caf {
    enum class ExitReason : uint8_t {
        NORMAL,
        ABNORMAL,
        SHUTDOWN,
        UNKNOWN
    };
}

#endif //NANO_CAF_2_190579E26BF84FB688E1A5A2704684E6
