//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/Coordinator.h>

namespace nano_caf {
    Coordinator::Coordinator(std::size_t numOfWorkers) {
        m_workers.reserve(numOfWorkers);
        for(std::size_t i=0; i<numOfWorkers; ++i) {
            m_workers.emplace_back(m_pendingTasks);
            m_workers[i].Launch();
        }
    }

    auto Coordinator::Schedule(Resumable* task) noexcept -> void {
        m_pendingTasks.Enqueue(task);
    }

    auto Coordinator::Shutdown() noexcept -> void {
        m_pendingTasks.Shutdown();
        for(auto&& worker : m_workers) {
            worker.WaitDone();
        }

        while(1) {
            auto* task = m_pendingTasks.Dequeue();
            if(task == nullptr) break;
            task->Release();
        }
    }
}

