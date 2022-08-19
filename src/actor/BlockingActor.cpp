//
// Created by Darwin Yuan on 2022/7/1.
//

#include <nano-caf/actor/BlockingActor.h>
#include <numeric>

namespace nano_caf {
    BlockingActor::BlockingActor(bool sync) : SchedActor(sync) {
        Run();
    }

    BlockingActor::~BlockingActor() {
        if(!m_closed.exchange(true)) {
            m_cv.WakeUp();
        }
        m_thread.join();
    }

    auto BlockingActor::Sleep() noexcept -> bool {
        m_cv.Wait([this]() {
            return !MailBox::IsBlocked() || m_closed;
        });
        return m_closed;
    }

    namespace {
        constexpr std::size_t NO_LIMITS = std::numeric_limits<std::size_t>::max();
    }
    auto BlockingActor::Run() noexcept -> void {
        m_thread = std::thread([this](){
            while(1) {
                if(Sleep()) {
                    SchedActor::Close(ExitReason::ABNORMAL);
                    break;
                }
                SchedActor::Resume(NO_LIMITS);
                if(MailBox::IsClosed()) {
                    m_closed = true;
                    break;
                }
            }
        });
    }

    auto BlockingActor::Send(Message* msg) noexcept -> Status {
        if(auto status = MailBox::SendMsg(msg); status != Status::BLOCKED) {
            return status;
        }

        m_cv.WakeUp();

        return Status::OK;
    }
}