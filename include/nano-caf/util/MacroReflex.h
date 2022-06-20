//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_08EA07E897DB446481574BA35F7BAC12
#define NANO_CAF_2_08EA07E897DB446481574BA35F7BAC12

#include <maco/basic.h>
#include <maco/map.h>
#include <maco/aggregate.h>
#include <nano-caf/util/TypeList.h>
#include <cstddef>
#include <utility>

///////////////////////////////////////////////////////////////////////////////////////////////
namespace nano_caf::detail {
    template<typename T, size_t ... I>
    auto DeduceMsgFieldTypes(std::index_sequence<I...>)
        -> TypeList<typename T::template __SeCrEtE_field<I, T>::Type...>;

    template<typename T>
    using MsgFieldsTypes = decltype(detail::DeduceMsgFieldTypes<T>(std::make_index_sequence<T::NuM_oF_fIeLdS>{}));
}

////////////////////////////////////////////////////////////////////////////////////////////////
#define __REFLEX_field_def__(n, x)                 \
__MACO_var_full(x);                              \
template <typename T>                           \
struct __SeCrEtE_field<n, T> {                  \
   using Type = decltype(T::__MACO_var_name(x)); \
   static auto Get(T& obj) -> decltype(auto) { return (obj.__MACO_var_name(x)); } \
   constexpr static auto Get(const T& obj) -> decltype(auto) { return (obj.__MACO_var_name(x)); } \
   constexpr static auto Name() -> const char* const { return __MACO_stringify(__MACO_var_name(x)); } \
};

////////////////////////////////////////////////////////////////////////////////////////////////
#define __REFLEX_fields(...) \
constexpr static size_t NuM_oF_fIeLdS = __MACO_pp_size(__VA_ARGS__); \
template<size_t N, typename T> struct __SeCrEtE_field; \
__MACO_map_i(__REFLEX_field_def__, 0, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////////////////////
#define __REFLEX_field_def_only__(x) __MACO_var_full(x);

////////////////////////////////////////////////////////////////////////////////////////////////
#define __REFLEX_fields_only(...) __MACO_map(__REFLEX_field_def_only__, __VA_ARGS__)

#endif //NANO_CAF_2_08EA07E897DB446481574BA35F7BAC12
