//
// Created by Darwin Yuan on 2022/6/24.
//

#include <nano-caf/actor/detail/ExpectOnceMsgHandlers.h>

namespace nano_caf::detail {
    auto ExpectOnceMsgHandlers::AddHandler(MsgTypeId id, detail::MsgHandler* handler) noexcept -> Status {
        if(handler == nullptr) return Status::NULL_PTR;
        m_handlers.emplace(id, handler);
        return Status::OK;
    }

    auto ExpectOnceMsgHandlers::HandleMsg(Message& msg) noexcept -> bool {
        if(m_handlers.empty()) return false;
        auto range = m_handlers.equal_range(msg.id);
        if(range.first == range.second) return false;
        std::for_each(range.first, range.second, [this, &msg](auto&& elem){
            elem.second->HandleMsg(msg);
        });
        m_handlers.erase(range.first, range.second);
        return true;
    }
}