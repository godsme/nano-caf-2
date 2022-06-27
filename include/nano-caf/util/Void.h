//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_68671F02BDB24C978CE625F91A0654CA
#define NANO_CAF_2_68671F02BDB24C978CE625F91A0654CA

namespace nano_caf {
    struct VoidType {};

    inline constexpr VoidType Void{};

    template<typename T>
    struct ValueTypeTrait {
        using ValueType = T;
    };

    template<>
    struct ValueTypeTrait<void> {
        using ValueType = VoidType;
    };

    template<typename T>
    struct ValueTypeTrait<T&> {
        using ValueType = T*;
    };

    template<typename T>
    using ValueTypeOf = typename ValueTypeTrait<T>::ValueType;
}

#endif //NANO_CAF_2_68671F02BDB24C978CE625F91A0654CA
