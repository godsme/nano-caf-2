//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_DB8084C4795145969EBFCA42C42DF97B
#define NANO_CAF_2_DB8084C4795145969EBFCA42C42DF97B

#include <nano-caf/msg/MsgDef.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/async/PromiseDoneNotifier.h>
#include <nano-caf/timer/TimerId.h>
#include <nano-caf/timer/TimerSpec.h>
#include <nano-caf/timer/TimeoutCallback.h>
#include <nano-caf/actor/ActorPtr.h>
#include <memory>

namespace nano_caf {
    CAF_def_message(ExitMsg, (reason, ExitReason));

    CAF_def_message(FutureDoneNotify, (notifier, std::shared_ptr<PromiseDoneNotifier>));

    CAF_def_message(StartTimerMsg,
                    (id, TimerId),
                    (actor, ActorWeakPtr),
                    (spec, TimerSpec),
                    (issue_time_point, std::chrono::steady_clock::time_point),
                    (is_periodic, bool),
                    (callback, TimeoutCallback));

    CAF_def_message(StopTimerMsg,
                    (actor, intptr_t),
                    (id, TimerId));

    CAF_def_message(ClearActorTimerMsg,
                    (actor, intptr_t));

    CAF_def_message(TimeoutMsg,
                    (id, TimerId),
                    (callback, TimeoutCallback));
}


#endif //NANO_CAF_2_DB8084C4795145969EBFCA42C42DF97B
