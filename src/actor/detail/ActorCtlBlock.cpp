//
// Created by Darwin Yuan on 2022/8/19.
//
#include <nano-caf/actor/detail/ActorCtlBlock.h>
#include <nano-caf/util/Assertions.h>

namespace nano_caf::detail {
    auto ActorCtlBlock::EnableSync() -> void {
        m_sync.emplace();
    }

    auto ActorCtlBlock::OnExit(ExitReason reason) -> void {
        if(m_sync) {
            m_sync->set_value(reason);
        }
    }
    auto ActorCtlBlock::Wait(ExitReason& reason) noexcept -> Status {
        CAF_ASSERT(m_sync);
        auto&& future = m_sync->get_future();
        future.wait();
        reason = future.get();
        return Status::OK;
    }
}