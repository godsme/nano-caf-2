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

        constexpr static Value CAUSE;
        constexpr static Cause VALUE;
    };


    template<typename T>
    struct Result : private Either<T, Status> {
        using Parent = Either<T, Status>;

        template<typename R>
        Result(R&& value) : Parent{std::forward<R>(value)} {}

        template<typename ... ARGS>
        Result(ResultTag::Value, ARGS&& ... args)
            : Parent{EitherTag::LEFT, std::forward<ARGS>(args)...} {}

        Result(ResultTag::Cause, Status status)
            : Parent{EitherTag::RIGHT, status} {}

        auto Ok() const noexcept -> bool {
            return Parent::Index() == 0;
        }

        auto GetStatus() const noexcept -> Status {
            return Parent::Index() == 1 ? *Parent::Right() : Status::OK;
        }

        auto operator*() const noexcept -> ValueTypeOf<T> const& {
            return *Parent::Left();
        }

        auto GetValue() const noexcept -> ValueTypeOf<T> const& {
            return *Parent::Left();
        }
    };
}

#endif //NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5
