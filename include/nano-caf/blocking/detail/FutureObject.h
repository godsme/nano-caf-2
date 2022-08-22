//
// Created by Darwin Yuan on 2022/8/22.
//

#ifndef NANO_CAF_2_C69126EA67124275B0F83E88763A4724
#define NANO_CAF_2_C69126EA67124275B0F83E88763A4724

#include <nano-caf/util/Result.h>
#include <nano-caf/util/Assertions.h>
#include <optional>
#include <mutex>

namespace nano_caf::blocking::detail {
    template<typename R>
    struct FutureObject {
        using Callback = std::function<auto (Result<R>) -> void>;

        template<typename CB, typename = std::enable_if_t<std::is_convertible_v<CB, Callback>>>
        auto SetCallback(CB&& cb) -> Status {
            std::unique_lock lock{m_mutex};
            CAF_ASSERT_TRUE_R(!notified && !m_cb, Status::INVALID_OP);
            if(m_result) {
                cb(std::move(*m_result));
                notified = true;
            } else {
                m_cb = std::forward<CB>(cb);
            }
            return Status::OK;
        }

        auto UnsetCallback() -> void {
            std::unique_lock lock{m_mutex};
            m_cb = nullptr;
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueTypeOf<R>>>>
        auto SetValue(T&& value) noexcept -> Status {
            return Set(std::forward<T>(value), ResultTag::VALUE);
        }

        auto OnFail(Status cause) noexcept -> Status {
            return Set(cause, ResultTag::CAUSE);
        }

        auto HasBeenSet() const noexcept  -> bool {
            std::unique_lock lock{m_mutex};
            return notified || m_cb;
        }

    private:
        template<typename T, typename TAG>
        auto Set(T&& value, TAG tag) noexcept -> Status {
            std::unique_lock lock{m_mutex};
            CAF_ASSERT_TRUE_R(!notified && !m_result, Status::INVALID_OP);
            if(m_cb) {
                m_cb(Result<R>{tag, std::forward<T>(value)});
                notified = true;
            } else {
                m_result.emplace(Result<R>{tag, std::forward<T>(value)});
            }
            return Status::OK;
        }

    private:
        mutable std::mutex m_mutex;
        Callback m_cb{};
        std::optional<Result<R>> m_result{};
        bool notified{};
    };
}

#endif //NANO_CAF_2_C69126EA67124275B0F83E88763A4724
