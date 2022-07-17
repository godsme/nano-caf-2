//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_1452027A9BD847BE908E6209A277CBC0
#define NANO_CAF_2_1452027A9BD847BE908E6209A277CBC0

#include <nano-caf/actor/detail/CancellableMsgHandler.h>
#include <nano-caf/msg/MsgTypeId.h>
#include <nano-caf/Status.h>
#include <map>

namespace nano_caf::detail {
    struct ExpectOnceMsgHandlers {
        auto AddHandler(MsgTypeId, std::shared_ptr<detail::CancellableMsgHandler> const&) noexcept -> Status;
        auto RemoveHandler(std::shared_ptr<detail::CancellableMsgHandler> const&) noexcept -> void;
        auto HandleMsg(Message& msg) noexcept -> bool;
        auto Empty() const noexcept -> bool {
            return m_handlers.empty();
        }
    private:
        std::multimap<MsgTypeId, std::shared_ptr<detail::CancellableMsgHandler>> m_handlers;
    };
}

#endif //NANO_CAF_2_1452027A9BD847BE908E6209A277CBC0
