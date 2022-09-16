//
// Created by Darwin Yuan on 2022/8/11.
//

#ifndef NANO_CAF_2_39CA41233EED45A0A7000DDF69571375
#define NANO_CAF_2_39CA41233EED45A0A7000DDF69571375

#include <spdlog/spdlog.h>
#include <nano-caf/Status.h>

namespace nano_caf::detail {
    template<typename T, typename = void>
    constexpr bool HasOperatorBool = false;

    template<typename T>
    constexpr bool HasOperatorBool<T, std::void_t<decltype(&T::operator bool)>> = true;

    template<typename T>
    inline constexpr auto SuccCheck(T&& value) -> std::pair<bool, Status> {
        using Type = std::decay_t<T>;
        if constexpr(std::is_same_v<Type, nano_caf::Status>) {
            return {value ==  Status::OK, value};
        } else if constexpr(std::is_pointer_v<Type>) {
            return {value != nullptr, Status::NULL_PTR};
        } else if constexpr(std::is_convertible_v<Type, bool> || HasOperatorBool<Type>) {
            return {(bool)value, Status::FAILED};
        } else {
            static_assert(std::is_convertible_v<Type, bool> ||
                          std::is_pointer_v<Type> ||
                          HasOperatorBool<Type> ||
                          std::is_same_v<Type, nano_caf::Status>);
            return {false, Status::INVALID_ARG};
        }
    }
}

#define __CAF_LOG_SUCC(expr, status, result) \
    SPDLOG_ERROR("{} failed, status = {}", #expr, status);  \
    result

#define __CAF_LOG_BOOL(expr, result) \
    SPDLOG_ERROR("{} expected to be true, but actually false", #expr);  \
    result

#define __CAF_LOG_PTR(p, result) \
    SPDLOG_ERROR("{} = nullptr", #p);                            \
    result

#define __CAF_GENERIC_ASSERT_R(expr, result) do {                       \
    if([[maybe_unused]] auto [result_, status_] = nano_caf::detail::SuccCheck(expr); !result_) {  \
        using Type = std::decay_t<decltype(expr)>;                      \
        if constexpr(std::is_same_v<Type, nano_caf::Status>) {          \
            __CAF_LOG_SUCC(expr, status_, result);                      \
        } else if constexpr(std::is_pointer_v<Type>) {                  \
            __CAF_LOG_PTR(expr, result);                                \
        } else if constexpr(std::is_convertible_v<Type, bool> || nano_caf::detail::HasOperatorBool<Type>) {        \
            __CAF_LOG_BOOL(expr, result);                               \
        }                                                               \
    }                                                                   \
} while(0)

#define CAF_ASSERT_R(expr, result) \
__CAF_GENERIC_ASSERT_R(expr, return result)

#define CAF_ASSERT(expr) \
__CAF_GENERIC_ASSERT_R(expr, return status_)

#define CAF_ASSERT_(expr) \
__CAF_GENERIC_ASSERT_R(expr, return {})

#define CAF_ASSERT__(expr) \
__CAF_GENERIC_ASSERT_R(expr, return)

#define CAF_ASSERT_WARN(expr) \
__CAF_GENERIC_ASSERT_R(expr, )

#endif //NANO_CAF_2_39CA41233EED45A0A7000DDF69571375
