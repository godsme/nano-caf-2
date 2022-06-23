//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_26F74FDF6C444048BB7CC8EE0771DDFF
#define NANO_CAF_2_26F74FDF6C444048BB7CC8EE0771DDFF

namespace nano_caf {
    struct Message;
}

namespace nano_caf::detail {
    struct MsgHandler {
        virtual auto HandleMsg(Message&) noexcept -> bool = 0;
        virtual ~MsgHandler() = default;
    };
}

#endif //NANO_CAF_2_26F74FDF6C444048BB7CC8EE0771DDFF
