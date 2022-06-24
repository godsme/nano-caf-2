//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
#define NANO_CAF_2_FAF61F286041433CA454B541C8DC233B

#include <nano-caf/async/detail/FutureObserver.h>
#include <nano-caf/async/PromiseDoneNotifier.h>
#include <nano-caf/async/FailHandler.h>
#include <nano-caf/util/Void.h>
#include <nano-caf/Status.h>
#include <variant>
#include <deque>
#include <memory>
#include <atomic>

namespace nano_caf::detail {
    template<typename R>
    struct FutureObject : PromiseDoneNotifier {
        using ObserverType = std::shared_ptr<FutureObserver<R>>;

        FutureObject() = default;

        auto RegisterObserver(ObserverType observer) noexcept -> void {
            if(Ready()) {
                NotifyObserver(observer);
            } else {
                m_observers.emplace_back(std::move(observer));
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
            if(!Ready()) {
                m_onFail = std::move(handler);
            } else if(m_value.index() == 2) {
                handler(std::get<2>(m_value));
            }
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueTypeOf<R>>>>
        auto SetValue(T&& value) noexcept -> void {
            m_value.template emplace<1>(std::forward<T>(value));
            OnReady();
        }

        auto SetValue() noexcept -> void {
            if constexpr(std::is_void_v<R>) {
                SetValue(Void::Instance());
            }
        }

        auto OnFail(Status cause) noexcept -> void {
            m_value.template emplace<2>(cause);
            OnReady();
        }

        auto Present() const noexcept -> bool {
            return m_value.index() == 1;
        }

        auto GetValue() noexcept -> decltype(auto) {
            return std::get<1>(m_value);
        }

        auto Commit() noexcept -> void override {
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

    private:
        auto OnReady() noexcept -> void {
            m_ready.store(true, std::memory_order_release);
        }

        auto Ready() const noexcept -> bool {
            return m_ready.load(std::memory_order_acquire);
        }

    protected:
        std::variant<std::monostate, ValueTypeOf<R>, Status> m_value{};
        std::deque<ObserverType> m_observers{};
        FailHandler m_onFail{};
        std::atomic_bool m_ready{};
    };
}

#endif //NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
