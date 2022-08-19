//
// Created by Darwin Yuan on 2022/6/17.
//
#include <nano-caf/util/SharedPtrCtlBlock.h>

namespace nano_caf::detail {
    auto SharedPtrCtlBlock::Release() noexcept -> void {
        if (m_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            m_objectDestructor(Get<void*>());
            ReleaseWeakRef();
        }
    }

    auto SharedPtrCtlBlock::ReleaseWeakRef() noexcept -> void {
        if (m_weakRefs == 1
            || m_weakRefs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            m_blockDestructor(this);
        }
    }

    auto SharedPtrCtlBlock::Lock() noexcept -> bool {
        auto count = m_refs.load();
        while (count != 0) {
            if (m_refs.compare_exchange_weak(count, count + 1,
                                             std::memory_order_relaxed)) {
                return true;
            }
        }

        return false;
    }
}