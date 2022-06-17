//
// Created by Darwin Yuan on 2022/6/17.
//
#include <nano-caf/util/SharedPtrCtlBlock.h>
#include <nano-caf/util/SharedPtr.h>

namespace nano_caf {

    auto SharedPtrCtlBlock::Release() noexcept -> void {
        if (m_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            m_objectDestructor(Get<void*>());
            ReleaseWeak();
        }
    }

    auto SharedPtrCtlBlock::ReleaseWeak() noexcept -> void {
        if (m_weakRefs == 1
            || m_weakRefs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            m_blockDestructor(this);
        }
    }

    auto SharedPtrCtlBlock::Lock() noexcept -> SharedPtr<SharedPtrCtlBlock> {
        auto count = m_refs.load();
        while (count != 0) {
            if (m_refs.compare_exchange_weak(count, count + 1,
                                             std::memory_order_relaxed)) {
                return SharedPtr<SharedPtrCtlBlock>{this, false};
            }
        }

        return SharedPtr<SharedPtrCtlBlock>{nullptr};
    }
}