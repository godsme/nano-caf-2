//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_6A8F961B582549B7822F64623E7C15D0
#define NANO_CAF_2_6A8F961B582549B7822F64623E7C15D0

#include <thread>

namespace nano_caf {
    struct WorkSharingQueue;
    struct Resumable;

    struct Worker {
        Worker(WorkSharingQueue& tasks)
            : m_tasks{tasks}
        {}

        auto Launch() noexcept -> void;

        auto WaitDone() noexcept -> void;

    private:
        auto Run() noexcept -> void;
        auto ScheduleTask(Resumable*) noexcept -> void;

    private:
        std::thread m_tid;
        WorkSharingQueue& m_tasks;
    };
}

#endif //NANO_CAF_2_6A8F961B582549B7822F64623E7C15D0
