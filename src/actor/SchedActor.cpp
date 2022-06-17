//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/SharedPtrCtlBlock.h>

namespace nano_caf {
    SchedActor::SchedActor() : m_initialized{0}, m_exit{0} {}

    auto SchedActor::HandleMsgInternal(Message& msg) noexcept -> TaskResult {
        UserDefinedHandleMessage(msg);
        return m_exit ? TaskResult::DONE : TaskResult::RESUME;
    }

    auto SchedActor::Resume() noexcept -> void {
        if(!m_initialized) {
            Init();
            m_initialized = 1;
        }

        MailBox::Consume([this](Message& msg) -> TaskResult {
            return HandleMsgInternal(msg);
        });
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