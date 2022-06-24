//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_AF515DE488C74271BFBFDB365BEC3FC6
#define NANO_CAF_2_AF515DE488C74271BFBFDB365BEC3FC6

#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/async/detail/InvokeResultType.h>
#include <type_traits>
#include <memory>

namespace nano_caf::detail {
    template<typename R, typename A, typename F, typename = std::enable_if_t<std::is_same_v<R, InvokeResultType<F, A>>>>
    struct FutureCallbackObject : FutureObject<R>, FutureObserver<A> {
        using Callback = std::decay_t<F>;
        using Super = FutureObject<R>;
        using ValueType = typename FutureObserver<A>::ValueType;

        FutureCallbackObject(F&& callback)
            : m_callback{std::forward<F>(callback)}
        {
        }

        ~FutureCallbackObject() = default;

    private:
        auto GetResult(ValueType const& value) -> decltype(auto) {
            if constexpr(std::is_void_v<R>) {
                if constexpr (std::is_void_v<A>) {
                    m_callback();;
                } else if constexpr(std::is_reference_v<A>) {
                    m_callback(*value);
                } else {
                    m_callback(value);;
                }
                return Void::Instance();
            } else {
                if constexpr (std::is_void_v<A>) {
                    return m_callback();;
                } else if constexpr(std::is_reference_v<A>) {
                   return  m_callback(*value);
                } else {
                    return m_callback(value);
                }
            }
        }

        auto OnFutureReady(ValueType const& value) noexcept -> void override {
            Super::SetValue(GetResult(value));
            Super::Commit();
        }

        auto OnFutureFail(Status cause) noexcept -> void override {
            Super::OnFail(cause);
            Super::Commit();
        }

    private:
        Callback m_callback;
    };
}

#endif //NANO_CAF_2_AF515DE488C74271BFBFDB365BEC3FC6
