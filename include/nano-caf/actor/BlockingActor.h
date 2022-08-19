//
// Created by Darwin Yuan on 2022/7/1.
//

#ifndef NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07
#define NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/CvNotifier.h>
#include <thread>

namespace nano_caf {
    struct BlockingActor : SchedActor {
        explicit BlockingActor(bool sync);
        ~BlockingActor();

    private:
        auto Send(Message*) noexcept -> Status override;
        auto Run() noexcept -> void;

    private:
        auto Sleep() noexcept -> bool;

    private:
        std::thread m_thread{};
        CvNotifier  m_cv{};
        std::atomic_bool m_closed{false};
    };
}

#endif //NANO_CAF_2_6DB968B7189A4512A31A5F6774F47D07
