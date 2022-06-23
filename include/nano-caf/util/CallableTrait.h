//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_9A5EF19B8E5045E6BDB9D97F2C045DF4
#define NANO_CAF_2_9A5EF19B8E5045E6BDB9D97F2C045DF4

#include <nano-caf/util/TypeList.h>
#include <type_traits>
#include <tuple>

namespace nano_caf::detail {
    template<typename T>
    struct FunctionTrait;

    template<typename R, typename ... ARGS>
    struct __FunctionTrait {
        using ResultType = R;
        using ArgsType = TypeList<ARGS...>;
        using DecayedArgsType = TypeList<std::decay_t<ARGS>...>;
        constexpr static size_t NUM_OF_ARGS = sizeof...(ARGS);

        template<std::size_t I>
        using Arg = std::tuple_element_t<I, std::tuple<ARGS...>>;
    };

    template<typename R, typename ... ARGS>
    struct FunctionTrait<R(ARGS...)> : __FunctionTrait<R, ARGS...> {
        constexpr static bool IS_VARIADIC = false;
        constexpr static bool IS_CONST = false;
    };

    template<typename R, typename ... ARGS>
    struct FunctionTrait<R(ARGS..., ...)> : __FunctionTrait<R, ARGS...> {
        constexpr static bool IS_VARIADIC = true;
        constexpr static bool IS_CONST = false;
    };

    template<typename C, typename R, typename ... ARGS>
    struct FunctionTrait<R (C::*)(ARGS...) const> : __FunctionTrait<R, ARGS...> {
        constexpr static bool IS_VARIADIC = false;
        constexpr static bool IS_CONST = true;
    };

    template<typename C, typename R, typename ... Args>
    struct FunctionTrait<R (C::*)(Args...)> : __FunctionTrait<R, Args...> {
        constexpr static bool IS_VARIADIC = false;
        constexpr static bool IS_CONST = false;
    };

    template<typename C, typename R, typename ... Args>
    struct FunctionTrait<R (C::*)(Args..., ...)> : __FunctionTrait<R, Args...> {
        constexpr static bool IS_VARIADIC = true;
        constexpr static bool IS_CONST = false;
    };

    template<typename C, typename R, typename ... Args>
    struct FunctionTrait<R (C::*)(Args..., ...) const> : __FunctionTrait<R, Args...> {
        constexpr static bool IS_VARIADIC = true;
        constexpr static bool IS_CONST = true;
    };
}

namespace nano_caf {
    template<typename T, typename = void>
    struct CallableTrait : detail::FunctionTrait<T> {};

    template<typename T>
    struct CallableTrait<T, std::enable_if_t<std::is_class_v<T>>> : detail::FunctionTrait<decltype(&T::operator())> {};

    template<typename F>
    using FirstArg = typename CallableTrait<F>::template Arg<0>;
}


#endif //NANO_CAF_2_9A5EF19B8E5045E6BDB9D97F2C045DF4
