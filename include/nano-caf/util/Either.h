//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_9AA1175D0EDE4437AEA421B0C748B9AD
#define NANO_CAF_2_9AA1175D0EDE4437AEA421B0C748B9AD

#include <nano-caf/util/Void.h>
#include <variant>
#include <type_traits>

namespace nano_caf {
    struct EitherTag {
        struct Left {};
        struct Right {};

        static constexpr Left LEFT{};
        static constexpr Right RIGHT{};
    };

    template<typename L, typename R>
    struct Either : private std::variant<ValueTypeOf<L>, ValueTypeOf<R>> {
    private:
        using Parent = std::variant<ValueTypeOf<L>, ValueTypeOf<R>>;
    public:
        using LeftType = ValueTypeOf<L>;
        using RightType = ValueTypeOf<R>;

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueTypeOf<L>> || std::is_convertible_v<T, ValueTypeOf<R>>>>
        Either(T&& value) noexcept: Parent{std::forward<T>(value)} {}

        template<typename ... ARGS>
        Either(EitherTag::Left, ARGS&& ... args) noexcept: Parent{std::in_place_index_t<0>{}, std::forward<ARGS>(args)...} {}
        template<typename ... ARGS>
        Either(EitherTag::Right, ARGS&& ... args) noexcept: Parent{std::in_place_index_t<1>{}, std::forward<ARGS>(args)...} {}

        Either(Either&& result) : Parent{static_cast<Parent&&>(result)} {}
        Either(Either const& result) : Parent{static_cast<Parent const&>(result)} {}

        auto operator=(Either const& result) noexcept -> Either& = default;
        auto operator=(Either&& result) noexcept -> Either& = default;

        auto Left() const noexcept -> LeftType const* {
            return std::get_if<0>(static_cast<Parent const*>(this));
        }

        auto Right() const noexcept -> RightType const* {
            return std::get_if<1>(static_cast<Parent const*>(this));
        }

        auto Index() const noexcept -> std::size_t {
            return Parent::index();
        }

        template<typename F>
        auto LeftMatch(F&& f) const noexcept -> decltype(auto) {
            auto* left = Left();
            return left ? f(*left) : *Right();
        }
    };
}

#endif //NANO_CAF_2_9AA1175D0EDE4437AEA421B0C748B9AD
