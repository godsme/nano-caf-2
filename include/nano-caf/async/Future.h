//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
#define NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B

#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/async/detail/FutureCallbackObject.h>
#include <nano-caf/async/detail/FutureCallbackProxy.h>
#include <nano-caf/async/FutureConcept.h>
#include <memory>

namespace nano_caf {
    template<typename T>
    struct Future final {
        using ObjectType = std::shared_ptr<detail::FutureObject<T>>;

        Future() noexcept = default;
        Future(ObjectType object) noexcept
            : m_object{std::move(object)}
        {}

        explicit operator bool() const {
            return (bool)m_object;
        }

        template<typename F, typename R = detail::InvokeResultType<F, T>, typename = std::enable_if_t<!IS_FUTURE<R>>>
        auto Then(F&& callback) noexcept -> Future<R> {
            if(!m_object) return {};
            auto cb = std::make_shared<detail::FutureCallbackObject<R, T, F>>(std::forward<F>(callback));
            m_object->RegisterObserver(cb);
            return Future<R>{cb};
        }

        template<typename F, typename R = detail::InvokeResultType<F, T>, typename = std::enable_if_t<IS_FUTURE<R>>>
        auto Then(F&& callback) noexcept -> R {
            if(!m_object) return {};
            using Object = detail::FutureCallbackProxy<FutureOfType<R>, T, F>;
            auto cb = std::make_shared<Object>(std::forward<F>(callback));
            m_object->RegisterObserver(cb);
            return R{cb};
        }

        template<typename F, typename = std::enable_if_t<std::is_invocable_r_v<void, F, Status>>>
        auto Fail(F&& on_fail) noexcept -> Future<T>& {
            if(!m_object) {
                on_fail(Status::NULL_PTR);
            } else {
                m_object->SetFailHandler(std::forward<F>(on_fail));
            }
            return *this;
        }

    private:
        template<typename, typename, typename>
        friend struct detail::FutureCallbackProxy;

    private:
        ObjectType m_object;
    };
}

#endif //NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
