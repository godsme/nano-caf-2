//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_704D56F9C18A439484FB84B126AEB51B
#define NANO_CAF_2_704D56F9C18A439484FB84B126AEB51B

#include <type_traits>

namespace nano_caf {
    struct AtomSignature {};

    template<typename T>
    constexpr bool IS_MSG_ATOM = std::is_base_of_v<AtomSignature, T>;
}

#endif //NANO_CAF_2_704D56F9C18A439484FB84B126AEB51B
