//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7
#define NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7

#include <nano-caf/util/SharedPtrDataHolder.h>
#include <nano-caf/util/SharedBlock.h>

namespace nano_caf {
    template <typename T>
    struct SharedPtr : SharedPtrDataHolder<T> {
    protected:
        using Parent = SharedPtrDataHolder<T>;
        using Parent::m_ptr;
        using Parent::CtlBlock;

    public:
        SharedPtr() noexcept = default;
        SharedPtr(T* ptr, bool addRef = true) noexcept : Parent{ptr} {
            if(ptr != nullptr && addRef) {
                CtlBlock()->AddRef();
            }
        }

        SharedPtr(SharedPtr const& p) noexcept : Parent{p.Get()}  {
            if(m_ptr != nullptr) {
                CtlBlock()->AddRef();
            }
        }

        SharedPtr(SharedPtr&& p) noexcept : Parent{p.Get()}  {
            p.m_ptr = nullptr;
        }

        auto operator=(SharedPtr const& another) noexcept -> SharedPtr& {
            Release();
            m_ptr = another.m_ptr;
            if(m_ptr != nullptr) {
                CtlBlock()->AddRef();
            }

            return *this;
        }

        auto operator=(SharedPtr&& another) noexcept -> SharedPtr& {
            std::swap(m_ptr, another.m_ptr);
            return *this;
        }

        ~SharedPtr() noexcept {
            Release();
        }

        auto Release() noexcept -> void {
            if (m_ptr) {
                CtlBlock()->Release();
                m_ptr = nullptr;
            }
        }
    };

    template <typename Tp, typename CTL_BLOCK, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto MakeShared(ARGS&& ... args) -> SharedPtr<Tp> {
        using Block = SharedBlock<Tp, CTL_BLOCK, MEM_ALLOCATOR>;
        auto* p = MEM_ALLOCATOR::Alloc(sizeof(Block));
        if(p == nullptr) return {};
        auto* block = new (p) Block{std::forward<ARGS>(args)...};
        return SharedPtr<Tp>{block->Get(), false};
    }

}

#endif //NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7
