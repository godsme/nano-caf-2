//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/SharedPtrCtlBlock.h>

namespace nano_caf {
    SchedActor::SchedActor(bool syncRequired) {
        if(syncRequired) {
            m_promise.emplace();
        }
    }

    auto SchedActor::OnExit(ExitReason reason) noexcept -> void {
        ExitHandler(reason);
        TrySync();
    }

    SchedActor::~SchedActor() {
        if(!m_exitReason) {
            OnExit(ExitReason::UNKNOWN);
        }
    }

    auto SchedActor::Wait(ExitReason& reason) noexcept -> Status {
        if(!m_promise) return Status::FAILED;
        auto&& future = m_promise->get_future();
        future.wait();
        reason = future.get();
        return Status::OK;
    }

    inline constexpr std::size_t MAX_CONSUMED_MSGS_PER_ROUND = 3;

    auto SchedActor::TrySync() noexcept -> void {
        if(m_promise) {
            m_promise->set_value(*m_exitReason);
        }
    }

    auto SchedActor::ExitCheck() noexcept -> TaskResult {
        if(m_exitReason) {
            OnExit(*m_exitReason);
            return TaskResult::DONE;
        }
        return TaskResult::SUSPENDED;
    }

    auto SchedActor::Init() noexcept -> TaskResult {
        InitHandler();
        return ExitCheck();
    }

    auto SchedActor::Resume() noexcept -> TaskResult {
        if(!inited) {
            inited = true;
            if(auto result = Init(); result != TaskResult::SUSPENDED) {
                return result;
            }
        }

        return MailBox::Consume(MAX_CONSUMED_MSGS_PER_ROUND, [this](Message& msg) -> TaskResult {
            m_currentMsg = &msg;
            UserDefinedHandleMessage(msg);
            m_currentMsg = nullptr;
            return ExitCheck();
        });
    }

    auto SchedActor::Exit_(ExitReason reason) -> void {
        if(!m_exitReason) {
            m_exitReason.emplace(reason);
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