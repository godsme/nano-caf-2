//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
#define NANO_CAF_2_FAF61F286041433CA454B541C8DC233B

#include <nano-caf/async/detail/FutureObserver.h>
#include <nano-caf/async/FailHandler.h>
#include <nano-caf/actor/OnActorContext.h>
#include <nano-caf/util/Void.h>
#include <nano-caf/Status.h>
#include <variant>
#include <deque>

namespace nano_caf::detail {
    template<typename R>
    struct FutureObject {
        using ValueType = typename ValueTypeTrait<R>::ValueType;
        using ObserverType = std::shared_ptr<FutureObserver<R>>;

        FutureObject() = default;

        auto RegisterObserver(ObserverType observer) noexcept -> void {
            if(m_value.index() == 0) {
                m_observers.emplace_back(std::move(observer));
            } else {
                NotifyObserver(observer);
            }
        }

        auto DeregisterObserver(FutureObserver<R>* observer) noexcept -> void {
            auto result = std::find_if(m_observers.begin(), m_observers.end(), [observer](auto&& obj) {
                return obj.get() == observer;
            });
            if(result != m_observers.end()) {
                m_observers.erase(result);
            }
        }

        auto SetFailHandler(FailHandler&& handler) noexcept -> void {
            m_onFail = std::move(handler);
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueType>>>
        auto SetValue(T&& value) noexcept -> void {
            m_value.template emplace<1>(std::forward<T>(value));
        }

        auto SetValue() noexcept -> void {
            m_value.template emplace<1>(Void::Instance());
        }

        auto OnFail(Status cause) noexcept -> void {
            m_value.template emplace<2>(cause);
        }

        auto GetValue() noexcept -> decltype(auto) {
            return std::get<1>(m_value);
        }

        auto Commit() noexcept -> void {
            switch (m_value.index()) {
                case 2:
                    if(m_onFail) m_onFail(std::get<2>(m_value));
                    [[fallthrough]];
                case 1:
                    NotifyObservers();
                    break;
                default: break;
            }
        }

    private:
        auto NotifyObserver(ObserverType& observer) noexcept -> void {
            switch(m_value.index()) {
                case 1: observer->OnFutureReady(std::get<1>(m_value)); break;
                case 2: observer->OnFutureFail(std::get<2>(m_value)); break;
                default: break; // should never be here.
            }
        }

        auto NotifyObservers() noexcept -> void {
            for (auto&& observer: m_observers) {
                NotifyObserver(observer);
            }
            m_observers.clear();
        }

    protected:
        std::variant<std::monostate, ValueType, Status> m_value{};
        FailHandler m_onFail{};
        std::deque<ObserverType> m_observers{};
    };
}

#endif //NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
