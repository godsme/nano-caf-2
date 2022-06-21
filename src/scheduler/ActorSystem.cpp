//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf {
    auto ActorSystem::Instance() noexcept -> ActorSystem& {
        static ActorSystem instance;
        return instance;
    }
}