//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
#define NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22

#include <nano-caf/actor/AbstractActor.h>
#include <nano-caf/util/CacheLineSize.h>
#include <nano-caf/actor/MailBox.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/Status.h>
#include <future>
#include <optional>

namespace nano_caf {
    namespace detail {
        struct ActorCtlBlock;
    }

    struct SchedActor
            : AbstractActor
            , protected MailBox {
        explicit SchedActor(bool syncRequired = false);
        ~SchedActor();

        auto Resume(std::size_t maxSchedMsgs) noexcept -> TaskResult;

    private:
        auto Init() noexcept -> TaskResult;

    private:
        auto ExitCheck() noexcept -> TaskResult;
        auto OnExit(ExitReason) noexcept -> void;
        auto HandleMsg(Message&) noexcept -> void;

    protected:
        auto Exit_(ExitReason reason) -> void;
        auto Close(ExitReason reason) -> void;
        auto CtlBlock() noexcept -> detail::ActorCtlBlock*;

    private:
        virtual auto InitHandler() noexcept -> void {}
        virtual auto ExitHandler(ExitReason) noexcept -> void {}
        virtual auto HandleUserDefinedMsg(Message&) noexcept -> bool { return false; };

    private:
        std::optional<ExitReason> m_exitReason;

    protected:
        mutable std::unique_ptr<Message> m_currentMsg{};
        bool m_inited{false};
    };
}

#endif //NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
