//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_3CE2F71848B044E6B283AD636446F95E
#define NANO_CAF_2_3CE2F71848B044E6B283AD636446F95E

#include <nano-caf/scheduler/queue.h>
#include <nano-caf/scheduler/resumable.h>
#include <condition_variable>
#include <mutex>

namespace nano_caf {
    struct work_sharing_queue {
        work_sharing_queue() = default;

        auto enqueue(resumable*) noexcept -> void;
        auto dequeue() noexcept -> resumable*;

    private:
        nano_caf::queue<resumable> tasks_{};
        std::condition_variable cv_{};
        std::mutex lock_{};
    };
}

#endif //NANO_CAF_2_3CE2F71848B044E6B283AD636446F95E
