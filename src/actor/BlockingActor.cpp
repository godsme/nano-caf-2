//
// Created by Darwin Yuan on 2022/7/1.
//

#include <nano-caf/actor/BlockingActor.h>
#include <numeric>

namespace nano_caf {
    BlockingActor::~BlockingActor() {
        // TODO:
    }

    auto BlockingActor::Run() noexcept -> void {
        if(running) return;
        m_thread = std::thread([this](){
            while(1) {
                {
                    std::unique_lock lock{m_lock};
                    m_cv.wait(lock, [this]() {
                        return !SchedActor::IsBlocked();
                    });
                }
                if(SchedActor::Resume(std::numeric_limits<std::size_t>::max()) == TaskResult::DONE && SchedActor::IsClosed()) break;
            }
        });
        running = true;
    }

    auto BlockingActor::SendMsg(Message* msg) noexcept -> Status {
        if(auto status = SchedActor::SendMsg(msg); status != Status::BLOCKED) {
            return status;
        }
        m_cv.notify_one();
        return Status::OK;
    }

    auto BlockingActor::Wait(ExitReason& reason) noexcept -> Status {
        m_thread.join();
        SchedActor::Wait(reason);
        return Status::OK;
    }
}