//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
#define NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B

#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/async/detail/FutureCallbackObject.h>
#include <nano-caf/async/FutureConcept.h>
#include <memory>

namespace nano_caf {
    struct OnActorContext;

    template<typename T>
    struct Future final {
        using ObjectType = std::shared_ptr<detail::FutureObject<T>>;
        using value_type = T;

        Future() noexcept = default;
        Future(OnActorContext& context, ObjectType object) noexcept
            : m_context{&context}
            , m_object{std::move(object)}
        {}

        template<typename F, typename R = std::invoke_result_t<F, T>, typename = std::enable_if_t<!std::is_void_v<T> && !IS_FUTURE<R>>>
        auto Then(F&& callback) noexcept -> Future<R> {
            if(m_context == nullptr || !m_object) return {};

            auto cb = std::make_shared<detail::FutureCallbackObject<T, R, F>>(*m_context, m_object, std::forward<F>(callback));
            return {*m_context, cb};
        }

//        template<typename F, typename R = std::invoke_result_t<F>, typename = std::enable_if_t<std::is_void_v<T> && !Is_Future<R>>>
//        auto then(F&& callback) noexcept -> future<std::invoke_result_t<F>> {
//            if(context_ == nullptr || !object_) return {};
//
//            auto cb = std::make_shared<detail::future_callback_object<R, F, void>>(*context_, object_, std::forward<F>(callback));
//            return {*context_, cb};
//        }
//
//        template<typename F, typename R = invoke_result_t<F, T>, typename = std::enable_if_t<!Is_Future<R>>>
//        auto then(F&& f) noexcept -> future<invoke_result_t<F, T>> {
//            if(context_ == nullptr || !object_) return {};
//
//            auto nf = [=](auto && value) mutable -> R { return std::apply(f, value); };
//            auto cb = std::make_shared<detail::future_callback_object<R, decltype(nf), T>>(*context_, object_, std::move(nf));
//
//            return future<R>{*context_, cb};
//        }
//
//        template<typename F, typename R = std::invoke_result_t<F, T>, typename = std::enable_if_t<Is_Future<R>>>
//        auto then(F&& callback) noexcept -> R {
//            if(context_ == nullptr || !object_) return {};
//
//            auto cb = std::make_shared<detail::future_proxy_object<R, F, T>>(*context_, object_, std::forward<F>(callback));
//            return R{*context_, cb};
//        }
//
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
//        ~future() noexcept {
//            if(object_ && object_.unique() && !object_->ready()) {
//                context_->add_future(object_);
//            }
//        }

    private:
//        template<typename R, typename F, typename A>
//        friend struct detail::future_proxy_object;
//
//        template<typename ... Xs>
//        friend struct detail::when_all_object;

        template<typename>
        friend struct Future;

    private:
        OnActorContext* m_context{};
        ObjectType m_object;
    };
}

#endif //NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
