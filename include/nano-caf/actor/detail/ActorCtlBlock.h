//
// Created by Darwin Yuan on 2022/8/19.
//

#ifndef NANO_CAF_2_31EF548E6AFF4BE8A813F5E0345432DF
#define NANO_CAF_2_31EF548E6AFF4BE8A813F5E0345432DF

#include <nano-caf/util/SharedPtrCtlBlock.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/Status.h>
#include <optional>
#include <future>

namespace nano_caf::detail {
    struct ActorCtlBlock : SharedPtrCtlBlock {
        using SharedPtrCtlBlock::SharedPtrCtlBlock;
        
        auto Wait(ExitReason& reason) noexcept -> Status;
        auto EnableSync() -> void;
        auto OnExit(ExitReason reason) -> void;

    private:
        std::optional<std::promise<ExitReason>> m_sync;
    };
}

#endif //NANO_CAF_2_31EF548E6AFF4BE8A813F5E0345432DF
