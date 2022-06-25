//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_F3349B7CB37449ADBA021294CB82B3E8
#define NANO_CAF_2_F3349B7CB37449ADBA021294CB82B3E8

#include <atomic>

namespace nano_caf {
    struct ShutdownNotifier {
        inline auto ShutdownNotified() const noexcept -> bool {
            return m_shutdown_.load(std::memory_order_acquire);
        }

        inline auto NotifyShutdown() -> void {
            m_shutdown_.store(true, std::memory_order_release);
        }
    private:
        std::atomic_bool m_shutdown_{false};
    };
}

#endif //NANO_CAF_2_F3349B7CB37449ADBA021294CB82B3E8
