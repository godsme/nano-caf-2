//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5
#define NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5

#include <nano-caf/util/Either.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    template<typename T>
    struct Result : private Either<T, Status> {
        using Parent = Either<T, Status>;

        template<typename R>
        Result(R&& value) : Parent{std::forward<R>(value)} {}

    };
}

#endif //NANO_CAF_2_D81D26BF872B4D7CAA3B01889C85A6D5
