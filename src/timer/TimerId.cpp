//
// Created by Darwin Yuan on 2022/7/15.
//

#include <nano-caf/timer/TimerId.h>
#include <atomic>
#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf {
    struct TimerId::Descriptor {
        enum class State : uint8_t {
            ACTIVE,
            EXPIRED,
            CANCELLED
        };

        Descriptor(intptr_t subscriber,
                   TimerSpec const& spec,
                   std::chrono::steady_clock::time_point const& issueTime,
                   std::size_t repeatTimes)
                : m_subscriber{subscriber}
                , m_spec{spec}
                , m_issueTime{issueTime}
                , m_repeatTimes{repeatTimes}
        {}

        auto AddRef() noexcept -> void {
            m_refs.fetch_add(1, std::memory_order_relaxed);
        }

        auto Release() noexcept -> void {
            if (m_refs == 1 || m_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete this;
            }
        }

        auto OnCancel() -> void {
            State active = m_active;
            while(!m_active.compare_exchange_weak(active, State::CANCELLED));
            if(active == State::ACTIVE) {
                ActorSystem::Instance().StopTimer(m_subscriber, this);
            }
        }

        auto OnExpire() -> bool {
            if(++m_expireTimes == m_repeatTimes) {
                State active = State::ACTIVE;
                return m_active.compare_exchange_strong(active, State::EXPIRED);
            }
            return true;
        }

        auto IsActive() const -> bool {
            return m_active == State::ACTIVE;
        }

        auto IsCancelled() const -> bool {
            return m_active == State::CANCELLED;
        }

        auto ShouldRepeat() const -> bool {
            return m_active == State::ACTIVE && m_expireTimes < m_repeatTimes;
        }

    private:
        std::atomic<std::size_t> m_refs{1};

    public:
        std::atomic<State> m_active{State::ACTIVE};
        intptr_t m_subscriber;
        TimerSpec m_spec;
        std::chrono::steady_clock::time_point m_issueTime;
        std::size_t m_repeatTimes;
        std::size_t m_expireTimes{0};
    };

    TimerId::TimerId(intptr_t subscriber, TimerSpec const& spec, std::chrono::steady_clock::time_point const& issueTime, std::size_t repeatTimes)
        : m_desc{new Descriptor{subscriber, spec, issueTime, repeatTimes}}
    {}

    TimerId::~TimerId() {
        if(m_desc) {
            m_desc->Release();
        }
    }

    auto TimerId::IsActive() const -> bool {
        return m_desc->IsActive();
    }

    auto TimerId::IsCancelled() const -> bool {
        return m_desc->IsCancelled();
    }

    auto TimerId::GetActorId() const -> intptr_t {
        return m_desc->m_subscriber;
    }

    auto TimerId::GetIssueTime() const -> std::chrono::steady_clock::time_point {
        return m_desc->m_issueTime;
    }

    auto TimerId::SetIssueTime(std::chrono::steady_clock::time_point time) -> void {
        m_desc->m_issueTime = time;
    }

    auto TimerId::GetTimeSpec() const -> TimerSpec const& {
        return m_desc->m_spec;
    }

    TimerId::TimerId(Descriptor* desc) : m_desc{desc} {
        if(m_desc != nullptr) {
            m_desc->AddRef();
        }
    }

    TimerId::TimerId(TimerId const& another) : m_desc{another.m_desc} {
        if(m_desc != nullptr) {
            m_desc->AddRef();
        }
    }

    TimerId::TimerId(TimerId&& another) : m_desc{another.m_desc} {
        if(m_desc != nullptr) {
            another.m_desc = nullptr;
        }
    }

    auto TimerId::operator=(TimerId const& another) -> TimerId& {
        if(m_desc == nullptr) {
            m_desc->Release();
        }
        m_desc = another.m_desc;
        if(m_desc != nullptr) {
            m_desc->AddRef();
        }

        return *this;
    }

    auto TimerId::operator=(TimerId&& another) -> TimerId& {
        std::swap(another.m_desc, m_desc);
        return *this;
    }

    auto TimerId::Cancel() -> void {
        if(m_desc) {
            m_desc->OnCancel();
        }
    }

    auto TimerId::OnExpire() -> bool {
        if(m_desc) {
            return m_desc->OnExpire();
        }
        return false;
    }

    auto TimerId::ShouldRepeat() const -> bool {
        return m_desc && m_desc->ShouldRepeat();
    }
}