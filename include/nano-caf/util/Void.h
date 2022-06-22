//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_68671F02BDB24C978CE625F91A0654CA
#define NANO_CAF_2_68671F02BDB24C978CE625F91A0654CA

namespace nano_caf {
    struct Void {
        static auto Instance() noexcept -> Void& {
            static Void instance{};
            return instance;
        }
    };

    template<typename T>
    struct ValueTypeTrait {
        using ValueType = T;
    };

    template<>
    struct ValueTypeTrait<void> {
        using ValueType = Void;
    };

    template<typename T>
    using ValueTypeOf = typename ValueTypeTrait<T>::ValueType;
}

#endif //NANO_CAF_2_68671F02BDB24C978CE625F91A0654CA
