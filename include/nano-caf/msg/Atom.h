//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_704D56F9C18A439484FB84B126AEB51B
#define NANO_CAF_2_704D56F9C18A439484FB84B126AEB51B

#include <type_traits>

namespace nano_caf {
    struct AtomSignature {};

    template<typename T>
    constexpr bool Is_Msg_Atom = std::is_base_of_v<AtomSignature, T>;

    template<typename T, typename = void>
    struct MsgKind {
        using Type = T;
    };

    template<typename T>
    struct MsgKind<T, std::enable_if_t<Is_Msg_Atom<T>>>  {
        using Type = typename T::MsgType;
    };

    template<typename T>
    using MsgType = typename MsgKind<T>::Type;
}

#endif //NANO_CAF_2_704D56F9C18A439484FB84B126AEB51B
