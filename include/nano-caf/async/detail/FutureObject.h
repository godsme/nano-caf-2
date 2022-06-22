//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
#define NANO_CAF_2_FAF61F286041433CA454B541C8DC233B

#include <nano-caf/async/detail/FutureObserver.h>
#include <nano-caf/actor/OnActorContext.h>
#include <nano-caf/util/Void.h>
#include <nano-caf/Status.h>
#include <variant>
#include <deque>

namespace nano_caf::detail {
    template<typename R>
    struct FutureObject : ObjectRegistry::Object {
        using ValueType = typename ValueTypeTrait<R>::ValueType;

        explicit FutureObject(OnActorContext& context) : m_context{context} {}

        auto RegisterObserver(FutureObserver<R>& observer) noexcept -> void {
            if(m_value.index() == 0) {
                m_observers.emplace_back(&observer);
            } else {
                NotifyObserver(observer);
            }
        }

        auto DeregisterObserver(FutureObserver<R>& observer) noexcept -> void {
            auto result = std::find(m_observers.begin(), m_observers.end(), &observer);
            if(result != m_observers.end()) {
                m_observers.erase(result);
            }
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueType>>>
        auto SetValue(T&& value) noexcept -> void {
            m_value.template emplace<1>(std::forward<T>(value));
        }

        auto OnFail(Status cause) noexcept -> void {
            m_value.template emplace<2>(cause);
        }

        auto GetValue() noexcept -> decltype(auto) {
            return std::get<1>(m_value);
        }

        auto Commit() noexcept -> void {
            if(m_value.index() != 0) {
                NotifyObservers();
                Destroy();
            }
        }

    private:
        auto Destroy() noexcept -> void {
            DeregisterFrom(m_context);
        }

        auto NotifyObserver(FutureObserver<R>& observer) noexcept -> void {
            switch(m_value.index()) {
                case 1: observer.OnFutureReady(std::get<1>(m_value)); break;
                case 2: observer.OnFutureFail(std::get<2>(m_value)); break;
                default: break; // should never be here.
            }
        }

        auto NotifyObservers() noexcept -> void {
            for (auto&& observer: m_observers) {
                NotifyObserver(*observer);
            }
            m_observers.clear();
        }

    protected:
        OnActorContext& m_context;
        std::variant<std::monostate, ValueType, Status> m_value;
        std::deque<FutureObserver<R>*> m_observers;
    };
}

#endif //NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
