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
            using ObjectType = detail::FutureCallbackProxy<FutureOfType<R>, T, F>;
            auto cb = std::make_shared<ObjectType>(std::forward<F>(callback));
            m_object->RegisterObserver(cb);
            return R{cb};
        }

//        template<typename F, typename R = std::invoke_result_t<F>, typename = std::enable_if_t<std::is_void_v<T> && Is_Future<R>>>
//        auto then(F&& callback) noexcept -> std::invoke_result_t<F> {
//            if(context_ == nullptr || !object_) return {};
//
//            auto cb = std::make_shared<detail::future_proxy_object<R, F, T>>(*context_, object_, std::forward<F>(callback));
//            return R{*context_, cb};
//        }
//
//        template<typename F, typename = std::enable_if_t<std::is_invocable_r_v<void, F, status_t>>>
//        auto fail(F&& on_fail) noexcept -> future<T>& {
//            if(!object_) {
//                on_fail(status_t::invalid_data);
//            } else {
//                object_->set_fail_handler(std::forward<F>(on_fail));
//            }
//            return *this;
//        }
//
//        auto cancel(status_t cause) noexcept -> void {
//            if(object_) {
//                object_->cancel(cause);
//                object_.release();
//            }
//        }
//
//        inline auto valid() const noexcept -> bool {
//            return static_cast<bool>(object_);
//        }
//

    private:
        template<typename, typename, typename>
        friend struct detail::FutureCallbackProxy;

    private:
        ObjectType m_object;
    };
}

#endif //NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
