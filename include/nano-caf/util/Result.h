//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5
#define NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5

#include <nano-caf/util/Either.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct ResultTag {
        struct Cause{};
        struct Value {};

        constexpr static Cause CAUSE{};
        constexpr static Value VALUE{};
    };


    template<typename T>
    struct Result : private Either<T, Status> {
        using Parent = Either<T, Status>;

        template<typename R, typename = std::enable_if_t<std::is_convertible_v<R, ValueTypeOf<T>> || std::is_convertible_v<R, Status>>>
        Result(R&& value) : Parent{std::forward<R>(value)} {}

        template<typename ... ARGS>
        Result(ResultTag::Value, ARGS&& ... args)
            : Parent{EitherTag::LEFT, std::forward<ARGS>(args)...} {}

        Result(ResultTag::Cause, Status status)
            : Parent{EitherTag::RIGHT, status} {}

        Result(Result&& result) : Parent{static_cast<Parent&&>(result)} {}
        Result(Result const& result) : Parent{static_cast<Parent const&>(result)} {}

        auto Ok() const noexcept -> bool {
            return Parent::Index() == 0;
        }

        auto GetStatus() const noexcept -> Status {
            return Parent::Index() == 1 ? *Parent::Right() : Status::OK;
        }

        operator ValueTypeOf<T> () const {
            return *Parent::Left();
        }

        auto operator*() const noexcept -> ValueTypeOf<T> const& {
            return *Parent::Left();
        }

        auto GetValue() const noexcept -> ValueTypeOf<T> const& {
            return *Parent::Left();
        }

        auto OrElse(ValueTypeOf<T>&& defaultValue) -> ValueTypeOf<T> {
            return Parent::Index() == 1 ? defaultValue : *Parent::Left();
        }

        friend inline auto operator==(Result const& lhs, Result const& rhs) -> bool {
            return Parent::operator==(lhs, rhs);
        }

        friend inline auto operator!=(Result const& lhs, Result const& rhs) -> bool {
            return Parent::operator!=(lhs, rhs);
        }

        friend inline auto operator==(Result const& lhs, ValueTypeOf<T> const& rhs) -> bool {
            switch(lhs.Index()) {
                case 0: return *lhs == rhs;
                default: return false;
            }
        }

        friend inline auto operator!=(Result const& lhs, ValueTypeOf<T> const& rhs) -> bool {
            return !operator==(lhs, rhs);
        }

        friend inline auto operator==(ValueTypeOf<T> const& lhs, Result const& rhs) -> bool {
            return operator==(rhs, lhs);
        }

        friend inline auto operator!=(ValueTypeOf<T> const& lhs, Result const& rhs) -> bool {
            return operator!=(rhs, lhs);
        }
    };
}

#endif //NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5
