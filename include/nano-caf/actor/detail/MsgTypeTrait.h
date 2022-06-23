//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_7A2315C98A0441F596FCFB49B4552A9B
#define NANO_CAF_2_7A2315C98A0441F596FCFB49B4552A9B

#include <nano-caf/util/TupleConcept.h>
#include <nano-caf/util/AggregateTrait.h>
#include <type_traits>

namespace nano_caf::detail {
    template <typename T, typename = void>
    struct MsgTypeTrait;

    template <typename T>
    struct MsgTypeTrait<T, std::enable_if_t<std::is_aggregate_v<T>>> : AggregateTrait<T> {
        using ResultType = void; // ???
    };

    template <typename T>
    struct MsgTypeTrait<T, std::enable_if_t<IS_TUPLE_V<typename T::TupleParent>>>  {
        using FieldsTypes = decltype(DeduceTupleTypes(std::declval<T>()));
        using ResultType = typename T::ResultType;

        template <typename F>
        static auto Call(T& obj, F&& f) {
            return std::apply(std::forward<F>(f), static_cast<typename T::TupleParent&>(obj));
        }
    };
}

#endif //NANO_CAF_2_7A2315C98A0441F596FCFB49B4552A9B
