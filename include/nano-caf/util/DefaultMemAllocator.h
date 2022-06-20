//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_F277D539216A460A83BF70DA0E3C0B2E
#define NANO_CAF_2_F277D539216A460A83BF70DA0E3C0B2E

namespace nano_caf {
    struct DefaultMemAllocator {
        static auto Alloc(std::size_t size) noexcept -> void* {
            return ::operator new(size);
        }

        static auto Free(void* p) noexcept -> void {
            ::operator delete(p);
        }
    };
}

#endif //NANO_CAF_2_F277D539216A460A83BF70DA0E3C0B2E
