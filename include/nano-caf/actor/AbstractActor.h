//
// Created by Darwin Yuan on 2022/7/1.
//

#ifndef NANO_CAF_2_67CC45D86B0B41CFBA69B284842F420B
#define NANO_CAF_2_67CC45D86B0B41CFBA69B284842F420B

#include <nano-caf/Status.h>
#include <nano-caf/actor/ExitReason.h>

namespace nano_caf {
    struct Message;

    struct AbstractActor {
        virtual auto SendMsg(Message*) noexcept -> Status = 0;
        virtual auto Wait(ExitReason& reason) noexcept -> Status = 0;
        virtual ~AbstractActor() = default;
    };
}

#endif //NANO_CAF_2_67CC45D86B0B41CFBA69B284842F420B
