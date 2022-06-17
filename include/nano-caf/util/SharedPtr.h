//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7
#define NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7

#include <nano-caf/util/CacheLineSize.h>
#include <string_view>

namespace nano_caf {
    struct SharedPtrCtlBlock;
    template <typename T>
    struct SharedPtr {
        explicit SharedPtr(T* ptr, bool addRef = true) noexcept : m_ptr{ptr} {
            if(ptr != nullptr && addRef) {
                CtlBlock()->AddRef();
            }
        }

        SharedPtr(SharedPtr const& p) noexcept : m_ptr{p.m_ptr} {
            if(m_ptr != nullptr) {
                CtlBlock()->AddRef();
            }
        }

        SharedPtr(SharedPtr&& p) noexcept : m_ptr{p.m_ptr} {
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

        auto operator->()  const noexcept -> T* { return m_ptr; }
        auto operator*()   const noexcept -> T& { return *m_ptr; }

        explicit operator bool() const noexcept {
            return m_ptr != nullptr;
        }

    private:
        auto CtlBlock() noexcept -> SharedPtrCtlBlock* {
            return reinterpret_cast<SharedPtrCtlBlock*>(reinterpret_cast<char*>(m_ptr) - CACHE_LINE_SIZE);
        }

    private:
        T* m_ptr;
    };
}

#endif //NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7
