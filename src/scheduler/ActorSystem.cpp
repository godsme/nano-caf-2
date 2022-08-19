//
// Created by Darwin Yuan on 2022/6/21.
//

#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/scheduler/Coordinator.h>
#include <nano-caf/timer/ActorTimerSystem.h>
#include <nano-caf/util/Assertions.h>

namespace nano_caf {
    struct ActorSystem::ActorSystemImpl
            : private ActorTimerSystem
            , private Coordinator {
        auto StartUp(std::size_t numOfWorkers) noexcept -> void {
            ActorTimerSystem::Start();
            Coordinator::StartUp(numOfWorkers);
        }
        auto Shutdown() noexcept -> void {
            Coordinator::Shutdown();
            ActorTimerSystem::Stop();
        }

        using Coordinator::Schedule;
        using ActorTimerSystem::StartTimer;
        using ActorTimerSystem::StopTimer;
        using ActorTimerSystem::ClearActorTimer;
    };

    auto ActorSystem::Instance() noexcept -> ActorSystem& {
        static ActorSystem instance;
        return instance;
    }

    auto ActorSystem::StartUp(std::size_t numOfWorkers) noexcept -> Status {
        if(m_impl == nullptr) {
            m_impl = new ActorSystemImpl{};
            CAF_ASSERT_NEW_PTR(m_impl);

            m_impl->StartUp(numOfWorkers);
        }

        return Status::OK;
    }

    auto ActorSystem::Shutdown() noexcept -> void {
        if(m_impl != nullptr) {
            m_impl->Shutdown();
            delete m_impl;
            m_impl = nullptr;
        }
    }

    auto ActorSystem::StartTimer( ActorPtr const& self,
                     TimerSpec const& spec,
                     std::size_t repeatTimes,
                     TimeoutCallback&& callback) -> Result<TimerId> {
        CAF_ASSERT_VALID_PTR(m_impl);
        return m_impl->StartTimer(self, spec, repeatTimes, std::move(callback));
    }

    auto ActorSystem::StopTimer(TimerId const& timerId) -> Status {
        CAF_ASSERT_VALID_PTR(m_impl);
        return m_impl->StopTimer(timerId);
    }

    auto ActorSystem::ClearActorTimer(intptr_t actorId) -> Status {
        CAF_ASSERT_VALID_PTR(m_impl);
        return m_impl->ClearActorTimer(actorId);
    }

    auto ActorSystem::Schedule(Resumable* resumable) noexcept -> Status {
        CAF_ASSERT_VALID_PTR(m_impl);
        return m_impl->Schedule(resumable);
    }
}