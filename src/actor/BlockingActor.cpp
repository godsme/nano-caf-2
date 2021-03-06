//
// Created by Darwin Yuan on 2022/7/1.
//

#include <nano-caf/actor/BlockingActor.h>
#include <numeric>

namespace nano_caf {
    BlockingActor::BlockingActor(bool) : SchedActor(true) {
        Run();
    }
    BlockingActor::~BlockingActor() {
        // TODO:
    }

    auto BlockingActor::Sleep() noexcept -> void {
        std::unique_lock lock{m_lock};
        m_cv.wait(lock, [this]() {
            return !SchedActor::IsBlocked();
        });
    }

    namespace {
        constexpr std::size_t NO_LIMITS = std::numeric_limits<std::size_t>::max();
    }
    auto BlockingActor::Run() noexcept -> void {
        m_thread = std::thread([this](){
            while(1) {
                Sleep();
                SchedActor::Resume(NO_LIMITS);
                if(SchedActor::IsClosed()) break;
            }
        });
        m_thread.detach();
    }

    auto BlockingActor::SendMsg(Message* msg) noexcept -> Status {
        if(auto status = SchedActor::SendMsg(msg); status != Status::BLOCKED) {
            return status;
        }
        m_cv.notify_one();
        return Status::OK;
    }

    auto BlockingActor::Wait(ExitReason& reason) noexcept -> Status {
        SchedActor::Wait(reason);
        return Status::OK;
    }
}