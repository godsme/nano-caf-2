//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/work_sharing_queue.h>

namespace nano_caf {
    auto work_sharing_queue::enqueue(resumable* task) noexcept -> void {
        if(task == nullptr) return;
        {
            std::unique_lock lock{lock_};
            tasks_.enqueue(task);
        }
        cv_.notify_one();
    }

    auto work_sharing_queue::dequeue() noexcept -> resumable * {
        std::unique_lock lock{lock_};
        cv_.wait(lock, [this]{ return !tasks_.empty(); });
        return tasks_.dequeue();
    }
}