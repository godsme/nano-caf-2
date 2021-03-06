//
// Created by Darwin Yuan on 2022/7/1.
//

#ifndef NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07
#define NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07

#include <nano-caf/actor/AbstractActor.h>
#include <nano-caf/actor/SchedActor.h>
#include <thread>

namespace nano_caf {
    struct BlockingActor : AbstractActor, protected SchedActor {
        explicit BlockingActor(bool);
        ~BlockingActor();

    private:
        auto SendMsg(Message*) noexcept -> Status override;
        auto Wait(ExitReason&) noexcept -> Status override;

        auto Run() noexcept -> void;
    private:
        auto Sleep() noexcept -> void;

    private:
        std::thread m_thread{};
        std::mutex m_lock{};
        std::condition_variable m_cv{};
    };
}

#endif //NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07
