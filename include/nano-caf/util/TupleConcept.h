//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_E638F9E86F244D2782D1ACE812FC2137
#define NANO_CAF_2_E638F9E86F244D2782D1ACE812FC2137

#include <nano-caf/util/TypeList.h>
#include <type_traits>
#include <tuple>

namespace nano_caf {
    template <typename> struct IS_TUPLE: std::false_type {};
    template <typename ...T> struct IS_TUPLE<std::tuple<T...>>: std::true_type {};

    template<typename T>
    inline constexpr bool IS_TUPLE_V = IS_TUPLE<T>::value;

    template <typename ... Ts>
    auto DeduceTupleTypes(std::tuple<Ts...>) -> TypeList<Ts...>;
}

#endif //NANO_CAF_2_E638F9E86F244D2782D1ACE812FC2137
