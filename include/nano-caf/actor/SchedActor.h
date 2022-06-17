//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
#define NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22

#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/util/CacheLineSize.h>
#include <nano-caf/actor/MailBox.h>
#include <nano-caf/actor/ExitReason.h>

namespace nano_caf {
    struct SharedPtrCtlBlock;

    struct SchedActor : private MailBox, Resumable {
        SchedActor();
        auto Resume() noexcept -> TaskResult override;

    private:
        auto AddRef() noexcept -> void override;
        auto Release() noexcept -> void override;

    private:
        auto CtlBlock() noexcept -> SharedPtrCtlBlock*;
        auto Close() noexcept -> void;

    protected:
        auto Exit_(ExitReason reason) -> void {
            if(!m_exit) {
                m_exit = 1;
                m_reason = reason;
            }
        }
    private:
        virtual auto Init() noexcept -> void {}
        virtual auto UserDefinedHandleMessage(Message&) noexcept -> void {}

    private:
        uint8_t m_initialized:1;
        uint8_t m_exit:1;
        ExitReason m_reason;
    };
}

#endif //NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
