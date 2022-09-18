//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_6021331FF3284E0FBAD637054B062B4D
#define NANO_CAF_2_6021331FF3284E0FBAD637054B062B4D

#include <stdint.h>

namespace nano_caf {
    struct Status {
        enum E : uint32_t {
            OK,
            FAILED,
            TIMEOUT,
            OUT_OF_MEM,
            NULL_PTR,
            NULL_ACTOR,
            INVALID_ARG,
            INVALID_OP,
            CLOSED,
            DISCARDED,
            SHUTDOWN,
            BLOCKED
        };

        constexpr Status(E value) : m_value{value} {}
        constexpr explicit operator E() const { return m_value; }

        inline friend constexpr auto operator==(Status const& lhs, Status const& rhs) -> bool {
            return lhs.m_value == rhs.m_value;
        }
        inline friend constexpr auto operator!=(Status const& lhs, Status const& rhs) -> bool {
            return !operator==(lhs, rhs);
        }
    private:
        E m_value;
    };

}

#endif //NANO_CAF_2_6021331FF3284E0FBAD637054B062B4D
