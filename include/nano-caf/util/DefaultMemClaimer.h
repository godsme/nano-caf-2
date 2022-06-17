//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_BE5BF01F3FAF410B93CE6A5F52BCE1AC
#define NANO_CAF_2_BE5BF01F3FAF410B93CE6A5F52BCE1AC

namespace nano_caf {
    struct DefaultMemClaimer {
        static auto Claim(void* p) noexcept -> void {
            ::operator delete(p);
        }
    };
}

#endif //NANO_CAF_2_BE5BF01F3FAF410B93CE6A5F52BCE1AC
