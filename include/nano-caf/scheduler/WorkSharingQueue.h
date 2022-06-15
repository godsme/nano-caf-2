//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_6FEB09B36F8246E18D93B01878B398C5
#define NANO_CAF_2_6FEB09B36F8246E18D93B01878B398C5

#include <nano-caf/scheduler/Queue.h>
#include <nano-caf/scheduler/Resumable.h>
#include <condition_variable>
#include <mutex>

namespace nano_caf {
    struct WorkSharingQueue {
        WorkSharingQueue() = default;

        auto Enqueue(Resumable*) noexcept -> void;
        auto Dequeue() noexcept -> Resumable*;
        auto Shutdown() noexcept -> void;
        auto CleanUp() noexcept -> void;

        ~WorkSharingQueue();
    private:
        nano_caf::Queue<Resumable> m_tasks{};
        std::condition_variable m_cv{};
        std::mutex m_lock{};
        bool m_shutdown{};
    };
}

#endif //NANO_CAF_2_6FEB09B36F8246E18D93B01878B398C5
