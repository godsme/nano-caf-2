//
// Created by Darwin Yuan on 2022/7/1.
//

#ifndef NANO_CAF_2_67CC45D86B0B41CFBA69B284842F420B
#define NANO_CAF_2_67CC45D86B0B41CFBA69B284842F420B

#include <nano-caf/Status.h>

namespace nano_caf {
    struct Message;

    struct AbstractActor {
        virtual auto Send(Message*) noexcept -> Status = 0;
        virtual ~AbstractActor() = default;
    };
}

#endif //NANO_CAF_2_67CC45D86B0B41CFBA69B284842F420B
