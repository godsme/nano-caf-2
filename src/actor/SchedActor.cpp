//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/SharedPtrCtlBlock.h>

namespace nano_caf {
    SchedActor::SchedActor(bool syncRequired)
        : m_initialized{0}, m_exit{0}, m_syncRequired{syncRequired} {
        if(m_syncRequired) {
            new (&m_sync.m_promise) std::promise<ExitReason>{};
        }
    }

    SchedActor::~SchedActor() {
        if(!m_exit) {
            ExitHandler(ExitReason::UNKNOWN);
        }

        if(m_syncRequired) {
            m_sync.m_promise.~promise<ExitReason>();
        }
    }

    auto SchedActor::Wait(ExitReason& reason) noexcept -> Status {
        if(!m_syncRequired) return Status::FAILED;
        auto&& future = m_sync.m_promise.get_future();
        future.wait();
        reason = future.get();
        return Status::OK;
    }

    inline constexpr std::size_t MAX_CONSUMED_MSGS_PER_ROUND = 3;

    inline auto SchedActor::TrySync() noexcept -> void {
        if(m_syncRequired) {
            m_sync.m_promise.set_value(m_reason);
        }
    }

    auto SchedActor::ExitCheck() noexcept -> TaskResult {
        if(m_exit) {
            ExitHandler(m_reason);
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
        if(!m_exit) {
            m_exit = 1;
            m_reason = reason;
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