//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_9EED4AC901964CB6AD56FE5D7717A720
#define NANO_CAF_2_9EED4AC901964CB6AD56FE5D7717A720

namespace nano_caf {
    struct PromiseDoneNotifier {
        virtual auto Commit() noexcept -> void = 0;
        virtual ~PromiseDoneNotifier() = default;
    };
}

#endif //NANO_CAF_2_9EED4AC901964CB6AD56FE5D7717A720
