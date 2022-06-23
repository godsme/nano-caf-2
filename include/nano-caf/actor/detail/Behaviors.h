//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_AB6DEBD742AB43D8AB87C8C0BB3A2603
#define NANO_CAF_2_AB6DEBD742AB43D8AB87C8C0BB3A2603

#include <nano-caf/actor/detail/MsgHandler.h>
#include <utility>
#include <tuple>

namespace nano_caf::detail {
    template<typename ... ARGS>
    struct Behaviors : MsgHandler {
        Behaviors(ARGS&& ... args) : m_behaviors{std::move(args)...} {}
        auto HandleMsg(Message& msg) noexcept -> void override {
            Handle(msg, std::make_index_sequence<sizeof...(ARGS)>{});
        }

    private:
        template<size_t ... I>
        auto Handle(Message& msg, std::index_sequence<I...>) -> bool {
            return (std::get<I>(m_behaviors)(msg) || ...);
        }

    private:
        std::tuple<ARGS...> m_behaviors;
    };
}

#endif //NANO_CAF_2_AB6DEBD742AB43D8AB87C8C0BB3A2603
