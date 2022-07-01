//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
#define NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22

#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/util/CacheLineSize.h>
#include <nano-caf/actor/MailBox.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/Status.h>
#include <future>
#include <optional>

namespace nano_caf {
    struct SharedPtrCtlBlock;

    struct SchedActor
            : protected MailBox {
        explicit SchedActor(bool syncRequired = false);
        ~SchedActor();

        using MailBox::SendMsg;

        auto Wait(ExitReason& reason) noexcept -> Status;

        auto Resume(std::size_t maxSchedMsgs) noexcept -> TaskResult;

    private:
        auto Init() noexcept -> TaskResult;

    private:
        auto ExitCheck() noexcept -> TaskResult;
        auto TrySync() noexcept -> void;
        auto OnExit(ExitReason reason) noexcept -> void;

    protected:
        auto Exit_(ExitReason reason) -> void;

    private:
        virtual auto InitHandler() noexcept -> void {}
        virtual auto ExitHandler(ExitReason) noexcept -> void {}
        virtual auto UserDefinedHandleMessage(Message&) noexcept -> void {}

    private:
        std::optional<std::promise<ExitReason>> m_promise;
        std::optional<ExitReason> m_exitReason;

    protected:
        Message* m_currentMsg{};
        bool m_inited{false};
    };
}

#endif //NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
