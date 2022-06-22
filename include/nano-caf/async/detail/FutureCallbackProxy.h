//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_2EB3A6A6420643489A1071CC6CA42EA1
#define NANO_CAF_2_2EB3A6A6420643489A1071CC6CA42EA1

#include <nano-caf/async/detail/FutureObject.h>
#include <type_traits>
#include <memory>

namespace nano_caf::detail {
    template<typename> struct Future;

    template<typename R, typename A, typename F>
    struct FutureCallbackProxy : FutureObject<R>, FutureObserver<A> {
        using Callback = std::decay_t<F>;
        using Super = FutureObject<R>;
        using ValueType = typename FutureObserver<A>::ValueType;

        FutureCallbackProxy(F&& callback)
            : m_callback{std::forward<F>(callback)}
        {
        }

        ~FutureCallbackProxy() = default;

    private:
        auto GetFuture(ValueType const& value) -> decltype(auto) {
            if constexpr (std::is_void_v<A>) {
                return m_callback();;
            } else {
                return m_callback(value);
            }
        }

        auto OnFutureReady(ValueType const& value) noexcept -> void override {
            auto&& result = GetFuture(value);
            if(!result) {
                OnFutureFail(Status::NULL_PTR);
            } else {
                auto&& future = result.m_object;
                for(auto&& observer : Super::m_observers) {
                    future->RegisterObserver(observer);
                }
                Super::m_observers.clear();
            }
        }

        auto OnFutureFail(Status cause) noexcept -> void override {
            Super::OnFail(cause);
            Super::Commit();
        }

    private:
        Callback m_callback;
    };
}

#endif //NANO_CAF_2_2EB3A6A6420643489A1071CC6CA42EA1
