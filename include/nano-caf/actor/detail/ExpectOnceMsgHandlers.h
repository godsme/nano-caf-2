//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_1452027A9BD847BE908E6209A277CBC0
#define NANO_CAF_2_1452027A9BD847BE908E6209A277CBC0

#include <nano-caf/msg/Message.h>
#include <nano-caf/actor/detail/MsgHandler.h>
#include <map>

namespace nano_caf::detail {
    struct ExpectOnceMsgHandlers {
        auto AddHandler(MsgTypeId, detail::MsgHandler*) noexcept -> Status;
        auto HandleMsg(Message& msg) noexcept -> bool;
        auto Empty() const noexcept -> bool {
            return m_handlers.empty();
        }
    private:
        std::multimap<MsgTypeId, std::unique_ptr<detail::MsgHandler>> m_handlers;
    };
}

#endif //NANO_CAF_2_1452027A9BD847BE908E6209A277CBC0
