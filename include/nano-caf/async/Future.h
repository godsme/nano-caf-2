//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
#define NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B

#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/async/detail/FutureCallbackObject.h>
#include <nano-caf/async/detail/FutureCallbackProxy.h>
#include <nano-caf/async/FutureConcept.h>
#include <nano-caf/util/Void.h>
#include <memory>

namespace nano_caf {
    namespace detail {
        struct FutureForwardTag {};
    }

    template<typename T>
    struct Future final {
    private:
        Future(detail::FutureForwardTag) : m_forward{true} {}
    public:
        using Object = ValueTypeOf<T>;
        using ObjectType = std::shared_ptr<detail::FutureObject<T>>;

        static auto Forward(Status status = Status::OK) -> Future<T> {
            return status == Status::OK ? Future{detail::FutureForwardTag{}} : Future{};
        }

        Future() noexcept = default;
        Future(ObjectType object) noexcept
            : m_object{std::move(object)}
        {}



        template<typename R, typename = std::enable_if_t<std::is_convertible_v<R, Object> && !std::is_convertible_v<R, ObjectType>>>
        Future(R&& value) noexcept
            : m_object{std::make_shared<detail::FutureObject<T>>(std::forward<R>(value))}
        {}

        explicit operator bool() const {
            return (bool)m_object;
        }

        template<typename F, typename R = detail::InvokeResultType<F, T>, typename = std::enable_if_t<!IS_FUTURE<R>>>
        auto Then(F&& callback) noexcept -> Future<R> {
            if(!m_object) return {};
            using ObjType = detail::FutureCallbackObject<R, T, F>;
            auto cb = std::make_shared<ObjType>(std::forward<F>(callback));
            if(cb == nullptr) return {};
            m_object->RegisterObserver(cb);
            return Future<R>{cb};
        }

        template<typename F, typename R = detail::InvokeResultType<F, T>, typename = std::enable_if_t<IS_FUTURE<R>>>
        auto Then(F&& callback) noexcept -> R {
            if(!m_object) return {};
            using Proxy = detail::FutureCallbackProxy<FutureOfType<R>, T, F>;
            auto cb = std::make_shared<Proxy>(std::forward<F>(callback));
            if(cb == nullptr) return {};
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

        auto Release() noexcept -> void {
            m_object.reset();
        }

        auto IsForward() const -> bool {
            return m_forward;
        }
    private:
        template<typename, typename, typename>
        friend struct detail::FutureCallbackProxy;

    private:
        ObjectType m_object;
        bool m_forward{false};
    };
}

#endif //NANO_CAF_2_8DE8315E6048472DAC32B2C3D5D6E43B
