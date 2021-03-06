//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_43A5CDBD68AF463EBD1FA2A6AA3ACBD0
#define NANO_CAF_2_43A5CDBD68AF463EBD1FA2A6AA3ACBD0

namespace nano_caf {
    template<typename T>
    struct Future;

    template<typename T>
    constexpr bool IS_FUTURE = false;

    template<typename T>
    constexpr bool IS_FUTURE<Future<T>> = true;

    template<typename T>
    struct FutureTrait;

    template<typename R>
    struct FutureTrait<Future<R>> {
        using Type = R;
    };

    template<typename T>
    using FutureOfType = typename FutureTrait<T>::Type;
}

#endif //NANO_CAF_2_43A5CDBD68AF463EBD1FA2A6AA3ACBD0
