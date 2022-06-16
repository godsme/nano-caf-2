//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_556622B36CEF4638AAE9E29AA60535D3
#define NANO_CAF_2_556622B36CEF4638AAE9E29AA60535D3

namespace nano_caf {

    template <typename T>
    struct IntrusivePtr {
        IntrusivePtr(T* ptr, bool addRef = true) : m_ptr{ptr} {
            if(ptr != nullptr && addRef) {
                ptr->AddRef();
            }
        }

        IntrusivePtr(IntrusivePtr const& p) : m_ptr{p.m_ptr} {
            if(m_ptr != nullptr) {
                p->AddRef();
            }
        }

        IntrusivePtr(IntrusivePtr&& p) : m_ptr{p.m_ptr} {
            p.m_ptr = nullptr;
        }

        ~IntrusivePtr() {
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

#endif //NANO_CAF_2_556622B36CEF4638AAE9E29AA60535D3
