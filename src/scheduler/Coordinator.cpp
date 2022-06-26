//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/Coordinator.h>

namespace nano_caf {
    auto Coordinator::StartUp(std::size_t numOfWorkers) noexcept -> void {
        if(working) return;
        if(numOfWorkers == 0) numOfWorkers = 1;
        m_pendingTasks = std::make_unique<WorkSharingQueue>();
        m_workers.reserve(numOfWorkers);
        for(std::size_t i=0; i<numOfWorkers; ++i) {
            m_workers.emplace_back(*m_pendingTasks);
            m_workers[i].Launch();
        }
        working = true;
    }

    auto Coordinator::Schedule(Resumable* task) noexcept -> Status {
        if(!working) return Status::CLOSED;
        m_pendingTasks->Enqueue(task);
        return Status::OK;
    }

    auto Coordinator::Shutdown() noexcept -> void {
        if(!working) return;
        m_pendingTasks->Shutdown();
        for(auto&& worker : m_workers) {
            worker.WaitDone();
        }
        m_workers.clear();
        m_pendingTasks.reset();
        working = false;
    }
}

