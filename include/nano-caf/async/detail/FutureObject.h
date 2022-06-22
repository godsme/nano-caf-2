//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
#define NANO_CAF_2_FAF61F286041433CA454B541C8DC233B

#include <nano-caf/async/PromiseDoneNotifier.h>
#include <nano-caf/async/detail/FutureObserver.h>
#include <nano-caf/Status.h>
#include <variant>
#include <deque>

namespace nano_caf::detail {
    template<typename R>
    struct FutureObject : PromiseDoneNotifier {
        FutureObject() = default;

        auto RegisterObserver(FutureObserver& observer) noexcept -> void {
            if(m_result.index() == 0) {
                m_observers.emplace_back(&observer);
            } else {
                NotifyObserver(observer);
            }
        }

        auto DeregisterObserver(FutureObserver& observer) noexcept -> void {
            auto result = std::find(m_observers.begin(), m_observers.end(), &observer);
            if(result != m_observers.end()) {
                m_observers.erase(result);
            }
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, R>>>
        auto SetValue(T&& value) noexcept -> void {
            m_result.emplace<1>(std::forward<T>(value));
        }

        auto OnFail(Status cause) noexcept -> void {
            m_result.emplace<2>(cause);
        }

        auto Commit() noexcept -> void override {
            if(m_result.index() != 0) {
                NotifyObservers();
                Destroy();
            }
        }

    private:
        auto NotifyObserver(FutureObserver& observer) noexcept -> void {
            switch(m_result.index()) {
                case 1: observer.OnFutureReady(); break;
                case 2: observer.OnFutureFail(m_result.get<2>()); break;
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
        std::variant<std::monostate, R, Status> m_result;
        std::deque<FutureObserver*> m_observers;
    };
}

#endif //NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
