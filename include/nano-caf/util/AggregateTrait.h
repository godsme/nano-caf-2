//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_6FBCC7A96BF14957A505864BDFDA6D70
#define NANO_CAF_2_6FBCC7A96BF14957A505864BDFDA6D70

#include <nano-caf/util/AggregateFieldsType.h>
#include <type_traits>

namespace nano_caf::detail {
    struct UniversalType {
        template<typename T> operator T() {}
    };

    template<typename T, typename = std::enable_if_t<std::is_aggregate_v<T>>, typename ... Ts>
    struct AggregateFieldsNumber {
        constexpr static size_t SIZE = sizeof...(Ts) - 1;
    };

    template<typename T, typename ... Ts>
    struct AggregateFieldsNumber<T, std::enable_if_t<std::is_aggregate_v<T> &&
            std::is_same_v<T, decltype(T{Ts{}...})>>, Ts...> {
        constexpr static size_t SIZE = AggregateFieldsNumber<T, void, Ts..., UniversalType>::SIZE;
    };
}

namespace nano_caf {
    template<typename T, typename = std::enable_if_t<std::is_aggregate_v<T>>>
    struct AggregateTrait {
        constexpr static size_t NUM_OF_FIELDS = detail::AggregateFieldsNumber<T>::SIZE;
        using FieldsTypes = typename detail::AggregateFieldsType<NUM_OF_FIELDS, T>::Type;

        template <typename F>
        static auto Call(T& obj, F&& f) {
            return detail::AggregateFieldsType<NUM_OF_FIELDS, T>::Call(obj, std::forward<F>(f));
        }
    };
}


#endif //NANO_CAF_2_6FBCC7A96BF14957A505864BDFDA6D70
