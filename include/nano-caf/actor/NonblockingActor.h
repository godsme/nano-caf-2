//
// Created by Darwin Yuan on 2022/7/1.
//

#ifndef NANO_CAF_2_0526AB600D854180BF2B388AD520436C
#define NANO_CAF_2_0526AB600D854180BF2B388AD520436C

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/scheduler/Resumable.h>


namespace nano_caf {
    struct NonblockingActor
            : SchedActor
            , private Resumable {
        using SchedActor::SchedActor;

    private:
        auto Send(Message*) noexcept -> Status override;

    private:
        auto Resume() noexcept -> TaskResult override;
        auto AddRef() noexcept -> void override;
        auto Release() noexcept -> void override;
    };
}
#endif //NANO_CAF_2_0526AB600D854180BF2B388AD520436C
