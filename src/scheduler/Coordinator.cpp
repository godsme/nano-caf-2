//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/Coordinator.h>

namespace nano_caf {
    auto Coordinator::StartUp(std::size_t numOfWorkers) noexcept -> void {
        if(working) return;
        m_workers.reserve(numOfWorkers);
        for(std::size_t i=0; i<numOfWorkers; ++i) {
            m_workers.emplace_back(m_pendingTasks);
            m_workers[i].Launch();
        }
    }

    auto Coordinator::Schedule(Resumable* task) noexcept -> Status {
        if(!working) return Status::CLOSED;
        m_pendingTasks.Enqueue(task);
        return Status::OK;
    }

    auto Coordinator::Shutdown() noexcept -> void {
        m_pendingTasks.Shutdown();
        for(auto&& worker : m_workers) {
            worker.WaitDone();
        }
        m_pendingTasks.CleanUp();
    }
}

