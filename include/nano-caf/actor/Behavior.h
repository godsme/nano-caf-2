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
#include <vector>
#include <spdlog/spdlog.h>

namespace nano_caf {
    struct Behavior {
        using Handler = std::shared_ptr<detail::MsgHandler>;
        using Handlers = std::vector<Handler>;
        template<typename ... ARGS>
        Behavior(ARGS&&...args) {
            static_assert(sizeof...(ARGS) > 0);
            static_assert(((CallableTrait<ARGS>::NUM_OF_ARGS > 0) && ...));
            m_handlers = std::make_shared<Handlers>(
                    std::initializer_list<Handler>{std::make_shared<detail::BehaviorObject<ARGS>>(std::move(args))...});
            if(m_handlers == nullptr) {
                SPDLOG_ERROR("create behaviors failed, out of memory");
                return;
            }
            for(auto&& handler : *m_handlers) {
                if(handler == nullptr) {
                    SPDLOG_ERROR("create behavior failed, out of memory");
                    m_handlers = nullptr;
                    return;
                }
            }
        }

        Behavior() = default;

        auto HandleMsg(Message& msg) noexcept -> bool {
            if(m_handlers == nullptr) {
                SPDLOG_WARN("empty behavior");
                return false;
            }

            for(auto&& handler : *m_handlers) {
                if(handler->HandleMsg(msg)) {
                    return true;
                }
            }

            return false;
        }

        operator bool() const noexcept {
            return m_handlers != nullptr;
        }

    private:

        Behavior(std::shared_ptr<Handlers> handlers) : m_handlers{handlers} {}

    public:
        friend auto operator+(Behavior const& lhs, Behavior const& rhs) -> Behavior {
            if(!lhs && !rhs) return {};
            if(!lhs) {
                return {rhs.m_handlers};
            }
            if(!rhs) {
                return {lhs.m_handlers};
            }
            auto handlers = std::make_shared<Handlers>(*lhs.m_handlers);
            if(handlers == nullptr) {
                SPDLOG_ERROR("create behaviors failed, out of memory");
                return {};
            }
            handlers->insert(handlers->end(), rhs.m_handlers->begin(), rhs.m_handlers->end());
            return {handlers};
        }

    private:
        std::shared_ptr<Handlers> m_handlers{};
    };
}

#endif //NANO_CAF_2_2239C8FB16B44B439B32BB3860323AF3
