//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3
#define NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3

#include <nano-caf/util/CallableTrait.h>
#include <nano-caf/util/Assertions.h>
#include <nano-caf/actor/detail/MsgHandler.h>
#include <nano-caf/actor/detail/BehaviorObject.h>
#include <nano-caf/actor/detail/Behaviors.h>
#include <memory>
#include <vector>

namespace nano_caf {
    struct Behavior {
        using Handler = std::shared_ptr<detail::MsgHandler>;
        using Handlers = std::vector<Handler>;
        template<typename ... ARGS>
        Behavior(ARGS&&...args) {
            static_assert(((CallableTrait<ARGS>::NUM_OF_ARGS > 0) && ...));
            if(sizeof...(ARGS) == 0) {
                return;
            }

            auto handlers = std::make_shared<Handlers>();
            CAF_ASSERT_NEW_PTR_VOID(handlers);

            auto behaviors = new detail::Behaviors{detail::BehaviorObject<ARGS>{std::move(args)}...};
            CAF_ASSERT_NEW_PTR_VOID(behaviors);

            handlers->emplace_back(behaviors);
            m_handlers = handlers;
        }

        Behavior() = default;

        auto HandleMsg(Message& msg) noexcept -> bool {
            if(m_handlers == nullptr) return false;
            for(auto&& handler : *m_handlers) {
                if(handler->HandleMsg(msg)) return true;
            }
            return false;
        }

        operator bool() const noexcept {
            return (bool)m_handlers;
        }

    private:
        Behavior(std::shared_ptr<Handlers> handlers)
            : m_handlers{std::move(handlers)}
        {}

    public:
        friend auto operator+(Behavior const& lhs, Behavior const& rhs) -> Behavior {
            if(!lhs) return {rhs.m_handlers};
            if(!rhs) return {lhs.m_handlers};

            auto handlers = std::make_shared<Handlers>(*lhs.m_handlers);
            CAF_ASSERT_VALID_PTR_R(handlers, {});
            handlers->insert(handlers->end(), rhs.m_handlers->begin(), rhs.m_handlers->end());
            return {handlers};
        }

    private:
        std::shared_ptr<Handlers> m_handlers{};
    };
}

#endif //NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3
