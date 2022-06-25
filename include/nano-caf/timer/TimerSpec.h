//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_FD13654B569E40D5888D939AC191D144
#define NANO_CAF_2_FD13654B569E40D5888D939AC191D144

#include <nano-caf/util/Either.h>
#include <nano-caf/timer/Duration.h>
#include <chrono>

namespace nano_caf {
    using TimerSpec = Either<Duration, std::chrono::steady_clock::time_point>;
}

#endif //NANO_CAF_2_FD13654B569E40D5888D939AC191D144
