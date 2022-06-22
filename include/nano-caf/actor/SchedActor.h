//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
#define NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22

#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/util/CacheLineSize.h>
#include <nano-caf/actor/MailBox.h>
#include <nano-caf/actor/OnActorContext.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/Status.h>
#include <future>
#include <variant>

namespace nano_caf {
    struct SharedPtrCtlBlock;

    struct SchedActor
            : protected MailBox
            , protected OnActorContext
            , Resumable {
        explicit SchedActor(bool syncRequired = false);
        ~SchedActor();

        auto Init() noexcept -> TaskResult;
        using MailBox::SendMsg;

        auto Wait(ExitReason& reason) noexcept -> Status;

    private:
        auto Resume() noexcept -> TaskResult override;

    private:
        auto ExitCheck() noexcept -> TaskResult;
        auto TrySync() noexcept -> void;

    private:
        auto AddRef() noexcept -> void override;
        auto Release() noexcept -> void override;

    private:
        auto CtlBlock() noexcept -> SharedPtrCtlBlock*;

    protected:
        auto Exit_(ExitReason reason) -> void;

    private:
        virtual auto InitHandler() noexcept -> void {}
        virtual auto ExitHandler(ExitReason) noexcept -> void {}
        virtual auto UserDefinedHandleMessage(Message&) noexcept -> void {}

    private:
        std::variant<std::monostate, std::promise<ExitReason>> m_promise;
        std::variant<std::monostate, ExitReason> m_exitReason;

    protected:
        Message* m_currentMsg{};
    };
}

#endif //NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
