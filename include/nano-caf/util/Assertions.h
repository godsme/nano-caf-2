//
// Created by Darwin Yuan on 2022/8/11.
//

#ifndef NANO_CAF_2_39CA41233EED45A0A7000DDF69571375
#define NANO_CAF_2_39CA41233EED45A0A7000DDF69571375

#include <nano-caf/Status.h>
#include <nano-caf/util/MemberDetector.h>

#if USE_SPDLOG
#include <spdlog/spdlog.h>
#endif

namespace nano_caf::detail {
    DEF_METHOD_DETECTOR(HasOperatorBool, bool, operator bool);

    template<typename T>
    constexpr bool CouldBeBool = std::is_same_v<T, bool> || HasOperatorBool<T>;

    template<typename T>
    constexpr bool IsStatus = std::is_same_v<T, nano_caf::Status> || std::is_same_v<T, nano_caf::Status::E>;

    template<typename T>
    inline constexpr auto SuccCheck(T&& value) -> bool {
        using Type = std::decay_t<T>;
        if constexpr(IsStatus<Type>) {
            return value == Status::OK;
        } else if constexpr(std::is_pointer_v<Type>) {
            return value != nullptr;
        } else if constexpr(CouldBeBool<Type>) {
            return (bool)value;
        } else {
            static_assert(std::is_pointer_v<Type> ||
                          CouldBeBool<Type> ||
                          std::is_same_v<Type, nano_caf::Status>);
            return false;
        }
    }

    template<typename T>
    inline constexpr auto GetStatus([[maybe_unused]] T&& value) -> Status {
        using Type = std::decay_t<T>;
        if constexpr(IsStatus<Type>) {
            return value;
        } else if constexpr(std::is_pointer_v<Type>) {
            return Status::NULL_PTR;
        } else if constexpr(CouldBeBool<Type>) {
            return Status::FAILED;
        } else {
            return Status::INVALID_ARG;
        }
    }

#if USE_SPDLOG
    template<typename ... Args>
    inline auto DoLog(spdlog::source_loc const& loc, Args&& ... args) -> void {
        spdlog::default_logger_raw()->log(loc, spdlog::level::err, std::forward<Args>(args)...);
    }

    template<typename T>
    inline auto Log(spdlog::source_loc const& loc, char const* expr, [[maybe_unused]] T&& result) -> void {
        using Type = std::decay_t<T>;
        if constexpr(IsStatus<Type>) {
            DoLog(loc, "assertion failed: ({}) = 0x{:08x}", expr, (Status::E)result);
        } else if constexpr(std::is_pointer_v<Type>) {
            DoLog(loc, "assertion failed: ({}) = nullptr", expr);
        } else if constexpr(CouldBeBool<Type>) {
            DoLog(loc, "assertion failed: ({}) = false", expr);
        }
    }
#endif
}

#if USE_SPDLOG
#define __CAF_LOG(expr, status) \
   nano_caf::detail::Log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, #expr, status)
#else
#define __CAF_LOG(expr, status)
#endif

#define __CAF_GENERIC_ASSERT_R(expr, result) do {             \
    if(auto&& status_ = expr; !detail::SuccCheck(status_)) {  \
         __CAF_LOG(expr, status_);                            \
        result;                                               \
    }                                                         \
} while(0)

#define CAF_ASSERT_R(expr, result) \
__CAF_GENERIC_ASSERT_R(expr, return result)

#define CAF_ASSERT(expr) \
__CAF_GENERIC_ASSERT_R(expr, return nano_caf::detail::GetStatus(status_))

#define _CAF_ASSERT(expr) \
__CAF_GENERIC_ASSERT_R(expr, return {})

#define __CAF_ASSERT(expr) \
__CAF_GENERIC_ASSERT_R(expr, return)

#define CAF_ASSERT_WARN(expr) \
__CAF_GENERIC_ASSERT_R(expr, )

#endif //NANO_CAF_2_39CA41233EED45A0A7000DDF69571375
