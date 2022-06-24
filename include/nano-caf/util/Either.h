//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_9AA1175D0EDE4437AEA421B0C748B9AD
#define NANO_CAF_2_9AA1175D0EDE4437AEA421B0C748B9AD

#include <nano-caf/util/Void.h>
#include <variant>
#include <type_traits>

namespace nano_caf {
    struct LeftTag {};
    struct RightTag {};

    struct EitherTag {
        static constexpr LeftTag LEFT;
        static constexpr RightTag RIGHT;
    };

    template<typename L, typename R>
    struct Either : private std::variant<ValueTypeOf<L>, ValueTypeOf<R>> {
    private:
        using Parent = std::variant<ValueTypeOf<L>, ValueTypeOf<R>>;
    public:
        using LeftType = ValueTypeOf<L>;
        using RightType = ValueTypeOf<R>;

        template<typename T>
        Either(T&& value) noexcept: Parent{std::forward<T>(value)} {}

        template<typename ... ARGS>
        Either(LeftTag, ARGS&& ... args) noexcept: Parent{std::in_place_index_t<0>{}, std::forward<ARGS>(args)...} {}
        template<typename ... ARGS>
        Either(RightTag, ARGS&& ... args) noexcept: Parent{std::in_place_index_t<1>{}, std::forward<ARGS>(args)...} {}

        auto Left() const noexcept -> LeftType const* {
            return std::get_if<0>(static_cast<Parent const*>(this));
        }

        auto Right() const noexcept -> RightType const* {
            return std::get_if<1>(static_cast<Parent const*>(this));
        }

        auto Index() const noexcept -> std::size_t {
            return Parent::index();
        }
    };
}

#endif //NANO_CAF_2_9AA1175D0EDE4437AEA421B0C748B9AD
