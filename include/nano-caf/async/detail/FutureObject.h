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
    private:
        enum : uint8_t {
            READY = 0x01,
            TIMEOUT = 0x02
        };

    public:
        using ObserverType = std::shared_ptr<FutureObserver<R>>;

        FutureObject() = default;

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueTypeOf<R>>>>
        FutureObject(T&& value) noexcept {
            SetValue(std::forward<T>(value));
        }

        auto RegisterObserver(ObserverType observer) noexcept -> void {
            if(Ready()) {
                NotifyObserver(observer);
            } else {
                m_observers.emplace_back(observer);
            }
        }

        auto SetFailHandler(FailHandler&& handler) noexcept -> void {
            if(!Ready()) {
                m_onFail = std::move(handler);
            } else if(m_value.index() == 2) {
                handler(std::get<2>(m_value));
            }
        }

        auto SetFailHandler(FailHandler const& handler) noexcept -> void {
            if(!Ready()) {
                m_onFail = handler;
            } else if(m_value.index() == 2) {
                handler(std::get<2>(m_value));
            }
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T, ValueTypeOf<R>>>>
        auto SetValue(T&& value) noexcept -> bool {
            if(Ready()) return false;
            m_value.template emplace<1>(std::forward<T>(value));
            return OnReady();
        }

        auto SetValue() noexcept -> bool {
            if constexpr(std::is_void_v<R>) {
                return SetValue(Void);
            } else {
                return false;
            }
        }

        auto OnFail(Status cause) noexcept -> bool {
            if(Ready()) return false;
            m_value.template emplace<2>(cause);
            return OnReady();
        }

        auto OnTimeout() noexcept -> bool {
            uint8_t ready = 0;
            return m_ready.compare_exchange_strong(ready, TIMEOUT, std::memory_order_acq_rel);
        }

        auto Present() const noexcept -> bool {
            return m_value.index() == 1;
        }

        auto GetValue() noexcept -> decltype(auto) {
            return std::get<1>(m_value);
        }

        auto Commit() noexcept -> void override {
            switch(m_ready) {
                case TIMEOUT: {
                    NotifyError(Status::TIMEOUT);
                    NotifyTimeout();
                    break;
                }
                case READY: {
                    Notify();
                    break;
                }
                default:
                    break;
            }
        }

    private:
        auto Notify() noexcept -> void {
            switch (m_value.index()) {
                case 2:
                    NotifyError(std::get<2>(m_value));
                    [[fallthrough]];
                case 1:
                    NotifyObservers();
                    break;
                default: break;
            }
        }

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

        auto NotifyTimeout() noexcept -> void {
            for (auto&& observer: m_observers) {
                observer->OnFutureFail(Status::TIMEOUT);
            }

            m_observers.clear();
        }

    private:
        auto OnReady() noexcept -> bool {
            uint8_t ready = 0;
            return m_ready.compare_exchange_strong(ready, READY, std::memory_order_acq_rel);
        }

        auto Ready() const noexcept -> bool {
            return m_ready.load(std::memory_order_acquire) > 0;
        }

        auto NotifyError(Status status) -> void {
            if(m_onFail) {
                m_onFail(status);
                m_onFail = FailHandler{};
            }
        }

    protected:
        std::variant<std::monostate, ValueTypeOf<R>, Status> m_value{};
        std::deque<ObserverType> m_observers{};
        FailHandler m_onFail{};
        std::atomic<uint8_t> m_ready{};
    };
}

#endif //NANO_CAF_2_FAF61F286041433CA454B541C8DC233B
