//
// Created by Darwin Yuan on 2022/7/1.
//

#ifndef NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07
#define NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07

#include <nano-caf/actor/AbstractActor.h>
#include <nano-caf/actor/SchedActor.h>
#include <thread>
#include <atomic>

namespace nano_caf {
    struct BlockingActor : AbstractActor, protected SchedActor {
        explicit BlockingActor(bool) : SchedActor(true) {}
        ~BlockingActor();

        auto Run() noexcept -> void;
        auto SendMsg(Message*) noexcept -> Status override;
        auto Wait(ExitReason&) noexcept -> Status override;

    private:
        std::thread m_thread{};
        std::mutex m_lock{};
        std::condition_variable m_cv{};
        std::atomic_bool running{false};
    };
}

#endif //NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07
