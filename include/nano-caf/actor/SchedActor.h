//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
#define NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22

#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/util/CacheLineSize.h>
#include <nano-caf/actor/MailBox.h>
#include <nano-caf/actor/ExitReason.h>
#include <future>

namespace nano_caf {
    struct SharedPtrCtlBlock;

    struct SchedActor : protected MailBox, Resumable {
        explicit SchedActor(bool syncRequired = false);
        ~SchedActor();

        using MailBox::SendMsg;

    private:
        auto Resume() noexcept -> TaskResult override;

    private:
        auto AddRef() noexcept -> void override;
        auto Release() noexcept -> void override;

    private:
        auto CtlBlock() noexcept -> SharedPtrCtlBlock*;

    protected:
        auto Exit_(ExitReason reason) -> void;

    private:
        virtual auto Init() noexcept -> void {}
        virtual auto ExitHandler(ExitReason) noexcept -> void {}
        virtual auto UserDefinedHandleMessage(Message&) noexcept -> void {}

    private:
        union Sync {
            Sync() {}
            ~Sync() {}
            std::promise<ExitReason> m_promise;
        } m_sync;

    protected:
        Message* m_currentMsg{};

    private:

        ExitReason m_reason;
        uint8_t m_initialized:1;
        uint8_t m_exit:1;
        uint8_t m_syncRequired:1;
    };
}

#endif //NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
