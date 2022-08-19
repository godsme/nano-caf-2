//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_5C2F027F5D9F49F6B495DA6A99A3166C
#define NANO_CAF_2_5C2F027F5D9F49F6B495DA6A99A3166C

#include <nano-caf/util/DefaultMemAllocator.h>

namespace nano_caf {
    template<typename OBJ, typename CTL_BLOCK, typename MEM_ALLOCATOR = DefaultMemAllocator>
    struct alignas(CACHE_LINE_SIZE) SharedBlock {
        static_assert(alignof(OBJ) <= CACHE_LINE_SIZE);
        static_assert(sizeof(CTL_BLOCK) <= CACHE_LINE_SIZE);

        template<typename ... ARGS>
        SharedBlock(ARGS&& ... args)
            : m_ctlBlock{DestroyObject, MEM_ALLOCATOR::Free}
        {
            new (m_object) OBJ{std::forward<ARGS>(args)...};
        }

        auto Get() noexcept -> OBJ* {
            return reinterpret_cast<OBJ*>(m_object);
        }

    private:
        static auto DestroyObject(void* obj) noexcept -> void {
            reinterpret_cast<OBJ*>(obj)->~OBJ();
        }

    private:
        CTL_BLOCK m_ctlBlock;
        alignas(CACHE_LINE_SIZE) char m_object[sizeof(OBJ)];
    };
}

#endif //NANO_CAF_2_5C2F027F5D9F49F6B495DA6A99A3166C
