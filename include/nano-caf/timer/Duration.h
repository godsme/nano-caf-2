//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_7E985FE6CD644E87BEF131C95C32D6C8
#define NANO_CAF_2_7E985FE6CD644E87BEF131C95C32D6C8

#include <cstdint>
#include <chrono>

namespace nano_caf {
    struct Duration {
        explicit Duration(uint64_t inMs) : m_inMs{inMs} {}

        template<typename Rep, typename Period>
        explicit Duration(std::chrono::duration<Rep, Period> timeout)
            : m_inMs(std::chrono::microseconds(timeout).count()) {}

        explicit operator std::chrono::microseconds() const {
            return std::chrono::microseconds(m_inMs);
        }

    private:
        uint64_t m_inMs;
    };
}

#endif //NANO_CAF_2_7E985FE6CD644E87BEF131C95C32D6C8
