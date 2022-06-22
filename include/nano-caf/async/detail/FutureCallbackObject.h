//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_AF515DE488C74271BFBFDB365BEC3FC6
#define NANO_CAF_2_AF515DE488C74271BFBFDB365BEC3FC6

#include <nano-caf/async/detail/FutureObject.h>
#include <type_traits>
#include <memory>

namespace nano_caf {
    struct OnActorContext;
};

namespace nano_caf::detail {
    template<typename R, typename A, typename F, typename = std::enable_if<std::is_invocable_v<F, A>>>
    struct FutureCallbackObject : FutureObject<R>, FutureObserver<A> {
        using Subject = std::shared_ptr<FutureObject<A>>;
        using Callback = std::decay_t<F>;
        using Super = FutureObject<R>;
        using ValueType = typename FutureObserver<A>::ValueType;

        FutureCallbackObject(OnActorContext& context, Subject subject, F&& callback)
            : FutureObject<R>{context}
            , m_subject{std::move(subject)}
            , m_callback{std::forward<F>(callback)}
        {
            if(m_subject) m_subject->RegisterObserver(*this);
        }

        ~FutureCallbackObject() {
            DetachSubject();
        }

    private:
        auto GetResult(ValueType const& value) -> decltype(auto) {
            if constexpr(std::is_void_v<R>) {
                if constexpr (std::is_void_v<A>) {
                    m_callback();;
                } else {
                    m_callback(value);;
                }
                return Void::Instance();
            } else {
                if constexpr (std::is_void_v<A>) {
                    return m_callback();;
                } else {
                    return m_callback(value);
                }
            }
        }

        auto DoCommit() noexcept -> void {
            Super::Commit();
            DetachSubject();
        }

        auto OnFutureReady(ValueType const& value) noexcept -> void override {
            Super::SetValue(GetResult(value));
            DoCommit();
        }

        auto OnFutureFail(Status cause) noexcept -> void override {
            Super::OnFail(cause);
            DoCommit();
        }

    private:
        auto DetachSubject() noexcept -> void {
            if(m_subject) {
                m_subject->DeregisterObserver(*this);
                m_subject.reset();
            }
        }

    private:
        Subject m_subject;
        Callback m_callback;
    };
}

#endif //NANO_CAF_2_AF515DE488C74271BFBFDB365BEC3FC6
