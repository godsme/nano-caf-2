//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_1A039D3DD157432DB7771310FA937933
#define NANO_CAF_2_1A039D3DD157432DB7771310FA937933

#include <cstddef>
#include <type_traits>

namespace nano_caf {
    template<typename ... Ts>
    struct TypeList {
        constexpr static size_t size = 0;
        template<template<typename T> typename F>
        using Transform = TypeList<>;

        template<typename T>
        using AppendType = TypeList<T>;

        template<typename T>
        using PrependType = TypeList<T>;

        template<template<typename ...> typename R>
        using ExportTo = R<>;
    };

    template<typename H, typename ... Ts>
    struct TypeList<H, Ts...> {
        constexpr static size_t size = sizeof...(Ts) + 1;
        using Head = H;
        using Tail = TypeList<Ts...>;

        template<template<typename T> typename F>
        using Transform = TypeList<F<H>, F<Ts>...>;

        template<template<typename T> typename F>
        constexpr static bool pred = (F<Ts>::value && ... && F<H>::value);

        template<typename T>
        using PrependType = TypeList<T, H, Ts...>;

        template<typename T>
        using AppendType = TypeList<H, Ts..., T>;

        template<template<typename ...> typename R>
        using ExportTo = R<H, Ts...>;
    };

    namespace detail {
        template<typename T, typename ... Ts>
        struct TypeListExists {
            constexpr static bool value = false;
        };

        template<typename T, typename H, typename ... Ts>
        struct TypeListExists<T, H, Ts...> {
            constexpr static bool value = std::is_same_v<T, H> || TypeListExists<T, Ts...>::value;
        };
    }

    template<typename T, typename TL>
    constexpr bool TypeExists = TL::template PrependType<T>::template ExportTo<detail::TypeListExists>::value;
}

#endif //NANO_CAF_2_1A039D3DD157432DB7771310FA937933
