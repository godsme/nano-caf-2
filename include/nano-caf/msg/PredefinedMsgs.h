//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_DB8084C4795145969EBFCA42C42DF97B
#define NANO_CAF_2_DB8084C4795145969EBFCA42C42DF97B

#include <nano-caf/msg/MsgDef.h>
#include <nano-caf/actor/ExitReason.h>
#include <nano-caf/async/PromiseDoneNotifier.h>
#include <memory>

namespace nano_caf {
    CAF_def_message(ExitMsg, (reason, ExitReason));

    CAF_def_message(FutureDoneNotify, (notifier, std::shared_ptr<PromiseDoneNotifier>));
}


#endif //NANO_CAF_2_DB8084C4795145969EBFCA42C42DF97B
