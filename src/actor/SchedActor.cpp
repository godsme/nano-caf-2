//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/SharedPtrCtlBlock.h>

namespace nano_caf {
    SchedActor::SchedActor(bool syncRequired) {
        if(syncRequired) {
            m_promise.emplace<std::promise<ExitReason>>();
        }
    }

    SchedActor::~SchedActor() {
        if(m_exitReason.index() == 0) {
            ExitHandler(ExitReason::UNKNOWN);
            MailBox::Close();
        }
    }

    auto SchedActor::Wait(ExitReason& reason) noexcept -> Status {
        if(m_promise.index() == 0) return Status::FAILED;
        auto&& future = std::get<1>(m_promise).get_future();
        future.wait();
        reason = future.get();
        return Status::OK;
    }

    inline constexpr std::size_t MAX_CONSUMED_MSGS_PER_ROUND = 3;

    inline auto SchedActor::TrySync() noexcept -> void {
        if(m_promise.index() == 1) {
            std::get<1>(m_promise).set_value(std::get<1>(m_exitReason));
        }
    }

    auto SchedActor::ExitCheck() noexcept -> TaskResult {
        if(m_exitReason.index() == 1) {
            ExitHandler(std::get<1>(m_exitReason));
            TrySync();
            return TaskResult::DONE;
        }
        return TaskResult::SUSPENDED;
    }

    auto SchedActor::Init() noexcept -> TaskResult {
        InitHandler();
        return ExitCheck();
    }

    auto SchedActor::Resume() noexcept -> TaskResult {
        return MailBox::Consume(MAX_CONSUMED_MSGS_PER_ROUND, [this](Message& msg) -> TaskResult {
            m_currentMsg = &msg;
            UserDefinedHandleMessage(msg);
            m_currentMsg = nullptr;
            return ExitCheck();
        });
    }

    auto SchedActor::Exit_(ExitReason reason) -> void {
        if(m_exitReason.index() == 0) {
            m_exitReason.emplace<1>(reason);
        }
    }

    auto SchedActor::CtlBlock() noexcept -> SharedPtrCtlBlock* {
        return reinterpret_cast<SharedPtrCtlBlock*>(reinterpret_cast<char*>(this) - CACHE_LINE_SIZE);
    }

    auto SchedActor::AddRef() noexcept -> void {
        return CtlBlock()->AddRef();
    }

    auto SchedActor::Release() noexcept -> void {
        return CtlBlock()->Release();
    }
}