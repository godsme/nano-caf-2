//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3
#define NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3

#include <nano-caf/util/CallableTrait.h>
#include <nano-caf/actor/detail/MsgHandler.h>
#include <nano-caf/actor/detail/BehaviorObject.h>
#include <nano-caf/actor/detail/Behaviors.h>
#include <memory>

namespace nano_caf {
    struct Behavior {
        template<typename ... ARGS>
        Behavior(ARGS&&...args) {
            static_assert(((CallableTrait<ARGS>::NUM_OF_ARGS > 0) && ...));
            m_ptr = std::shared_ptr<detail::MsgHandler>(new detail::Behaviors{detail::BehaviorObject<ARGS>{std::move(args)}...});
        }

        Behavior() = default;

        auto HandleMsg(Message& msg) noexcept -> bool {
            return m_ptr ? m_ptr->HandleMsg(msg) : false;
        }

        operator bool() const noexcept {
            return (bool)m_ptr;
        }

    private:
        std::shared_ptr<detail::MsgHandler> m_ptr{};
    };
}

#endif //NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3
