//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
#define NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22

#include <nano-caf/scheduler/Resumable.h>
#include <nano-caf/util/CacheLineSize.h>

namespace nano_caf {
    struct SharedPtrCtlBlock;

    struct SchedActor : Resumable {
        auto Resume() noexcept -> void override;

    private:
        auto AddRef() noexcept -> void override;
        auto Release() noexcept -> void override;

    private:
        auto CtlBlock() noexcept -> SharedPtrCtlBlock*;
    };
}

#endif //NANO_CAF_2_28C671DF7AB34E83BDED55878F4A9D22
