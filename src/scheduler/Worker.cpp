//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/Worker.h>
#include <nano-caf/scheduler/WorkSharingQueue.h>

namespace nano_caf {
    auto Worker::Run() noexcept -> void {
        while(1) {
            auto* task = m_tasks.Dequeue();
            if(task == nullptr) break;
            task->Resume();
            task->Release();
        }
    }

    auto Worker::Launch() noexcept -> void {
        m_tid = std::thread([this] {
            Run();
        });
    }

    auto Worker::WaitDone() noexcept -> void {
        m_tid.join();
    }
}