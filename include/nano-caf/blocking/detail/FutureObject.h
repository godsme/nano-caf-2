//
// Created by Darwin Yuan on 2022/8/22.
//

#ifndef NANO_CAF_2_C69126EA67124275B0F83E88763A4724
#define NANO_CAF_2_C69126EA67124275B0F83E88763A4724

#include <nano-caf/util/Result.h>
#include <optional>
#include <mutex>
#include "nano-caf/util/Assertions.h"

namespace nano_caf::blocking::detail {
    template<typename R>
    struct FutureObject {
        using Callback = std::function<auto (Result<R>) -> void>;

        template<typename CB, typename = std::enable_if_t<std::is_convertible_v<CB, Callback>>>
        auto SetCallback(CB&& cb) -> Status {
            std::unique_lock lock{m_mutex};
            CAF_ASSERT_TRUE_R(!stale && !m_cb, Status::INVALID_OP);
            if(m_result) {
                cb(std::move(*m_result));
                stale = true;
            } else {
                m_cb = std::forward<CB>(cb);
            }
            return Status::OK;
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueTypeOf<R>>>>
        auto SetValue(T&& value) noexcept -> Status {
            std::unique_lock lock{m_mutex};
            CAF_ASSERT_TRUE_R(!m_result, Status::INVALID_OP);
            m_result.emplace(Result<R>{ResultTag::VALUE, std::forward<T>(value)});
            TryNotify();
            return Status::OK;
        }

        auto OnFail(Status cause) noexcept -> Status {
            std::unique_lock lock{m_mutex};
            CAF_ASSERT_TRUE_R(!m_result, Status::INVALID_OP);
            m_result.emplace(Result<R>{ResultTag::CAUSE, cause});
            TryNotify();
            return Status::OK;
        }

    private:
        auto TryNotify() -> void {
            if(m_cb) {
                m_cb(std::move(*m_result));
                stale = true;
            }
        }

    private:
        std::mutex m_mutex;
        Callback m_cb;
        std::optional<Result<R>> m_result;
        bool stale{false};
    };
}

#endif //NANO_CAF_2_C69126EA67124275B0F83E88763A4724
