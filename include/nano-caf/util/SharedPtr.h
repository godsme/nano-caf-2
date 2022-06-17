//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7
#define NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7

namespace nano_caf {

    template <typename T>
    struct SharedPtr {
        SharedPtr(T* ptr, bool addRef = true) : m_ptr{ptr} {
            if(ptr != nullptr && addRef) {
                ptr->AddRef();
            }
        }

        SharedPtr(SharedPtr const& p) : m_ptr{p.m_ptr} {
            if(m_ptr != nullptr) {
                p->AddRef();
            }
        }

        SharedPtr(SharedPtr&& p) : m_ptr{p.m_ptr} {
            p.m_ptr = nullptr;
        }

        ~SharedPtr() {
            Release();
        }

        auto Release() -> void {
            if (m_ptr) {
                m_ptr->Release();
                m_ptr = nullptr;
            }
        }

        T* operator->()  const noexcept { return m_ptr; }
        T& operator*()   const noexcept { return *m_ptr; }

        explicit operator bool() const noexcept {
            return m_ptr != nullptr;
        }

    private:
        T* m_ptr;
    };
}

#endif //NANO_CAF_2_B6AA6A48734F4E79BCC6F6B079B426D7
