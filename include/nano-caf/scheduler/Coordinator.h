//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_15571182463F456FA8824BCD97DA5421
#define NANO_CAF_2_15571182463F456FA8824BCD97DA5421

#include <nano-caf/scheduler/WorkSharingQueue.h>
#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/scheduler/Worker.h>
#include <nano-caf/Status.h>
#include <vector>

namespace nano_caf {
    struct Coordinator {
        Coordinator() = default;

        auto StartUp(std::size_t numOfWorkers) noexcept -> void;
        auto Schedule(Resumable*) noexcept -> Status;
        auto Shutdown() noexcept -> void;

    private:
        WorkSharingQueue m_pendingTasks;
        std::vector<Worker> m_workers;
        bool working{};
    };
}

#endif //NANO_CAF_2_15571182463F456FA8824BCD97DA5421
