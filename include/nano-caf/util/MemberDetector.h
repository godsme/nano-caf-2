//
// Created by Darwin Yuan on 2022/8/27.
//

#ifndef NANO_CAF_2_3A0DDBDC91684AF7BD30742C61AE5C70
#define NANO_CAF_2_3A0DDBDC91684AF7BD30742C61AE5C70

#include <type_traits>

#define DEF_METHOD_DETECTOR(name, result, method, ...) \
template<typename T, typename = void> \
constexpr bool name = false; \
template<typename T> \
constexpr bool name<T, std::enable_if_t<std::is_invocable_r_v<result, decltype(&T::method), T*, ##__VA_ARGS__>>> = true

#define DEF_MEMBER_VAR_DETECTOR(name, result, var) \
template<typename T, typename = void> \
constexpr bool name = false; \
template<typename T> \
constexpr bool name<T, std::enable_if_t<std::is_same_v<result, std::decay_t<decltype(T::var)>>>> = true

#endif //NANO_CAF_2_3A0DDBDC91684AF7BD30742C61AE5C70
