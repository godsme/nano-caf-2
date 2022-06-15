//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_15571182463F456FA8824BCD97DA5421
#define NANO_CAF_2_15571182463F456FA8824BCD97DA5421

#include <nano-caf/scheduler/WorkSharingQueue.h>
#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/scheduler/Worker.h>
#include <vector>

namespace nano_caf {
    struct Coordinator {
        explicit Coordinator(std::size_t numOfWorkers);

        auto Schedule(Resumable*) noexcept -> void;
        auto Shutdown() noexcept -> void;

    private:
        WorkSharingQueue m_pendingTasks;
        std::vector<Worker> m_workers;
    };
}

#endif //NANO_CAF_2_15571182463F456FA8824BCD97DA5421
