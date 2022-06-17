//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_D1D5BDA44099401BACF7FA215DE6558E
#define NANO_CAF_2_D1D5BDA44099401BACF7FA215DE6558E

#include <nano-caf/util/SharedPtr.h>
#include <atomic>

inline constexpr std::size_t CACHE_LINE_SIZE = 64;

namespace nano_caf {
    struct SharedPtrCtlBlock {
        using ObjectDestructor = auto (*)(void*) noexcept -> void;
        using BlockDestructor = auto (*)(SharedPtrCtlBlock*) noexcept -> void;

        SharedPtrCtlBlock(ObjectDestructor objectDtor, BlockDestructor blockDtor) noexcept
            : m_objectDestructor{objectDtor}
            , m_blockDestructor{blockDtor}
        {}

        ~SharedPtrCtlBlock() noexcept = default;

        template<typename OBJ>
        auto Get() const noexcept -> OBJ* {
            return const_cast<OBJ*>(reinterpret_cast<OBJ const*>(reinterpret_cast<char const*>(this) + CACHE_LINE_SIZE));
        }

        auto UseCount() const noexcept -> std::size_t {
            return m_refs.load(std::memory_order_relaxed);
        }

        auto AddRef() noexcept -> void {
            m_refs.fetch_add(1, std::memory_order_relaxed);
        }

        auto AddWeakRef() noexcept -> void {
            m_weakRefs.fetch_add(1, std::memory_order_relaxed);
        }

        auto Release() noexcept -> void;
        auto ReleaseWeak() noexcept -> void;
        auto Lock() noexcept -> SharedPtr<SharedPtrCtlBlock>;

    private:
        std::atomic<std::size_t> m_refs{1};
        std::atomic<std::size_t> m_weakRefs{1};
        ObjectDestructor m_objectDestructor;
        BlockDestructor  m_blockDestructor;
    };

    static_assert(sizeof(SharedPtrCtlBlock) <= CACHE_LINE_SIZE);
}

#endif //NANO_CAF_2_D1D5BDA44099401BACF7FA215DE6558E
