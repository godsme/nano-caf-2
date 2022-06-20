//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_596CCED255394A3A9081033D515D8153
#define NANO_CAF_2_596CCED255394A3A9081033D515D8153

#include <nano-caf/util/CacheLineSize.h>
#include <string_view>

namespace nano_caf {
    struct SharedPtrCtlBlock;

    template <typename T>
    struct SharedPtrDataHolder {

        auto Get() const noexcept -> T* { return m_ptr; }
        auto operator->()  const noexcept -> T* { return m_ptr; }
        auto operator*()   const noexcept -> T& { return *m_ptr; }

        explicit operator bool() const noexcept {
            return m_ptr != nullptr;
        }

    protected:
        SharedPtrDataHolder() noexcept = default;
        explicit SharedPtrDataHolder(T* ptr) noexcept : m_ptr{ptr} {}

    protected:
        auto CtlBlock() noexcept -> SharedPtrCtlBlock* {
            return reinterpret_cast<SharedPtrCtlBlock*>(reinterpret_cast<char*>(m_ptr) - CACHE_LINE_SIZE);
        }

    protected:
        T* m_ptr{};
    };
}

#endif //NANO_CAF_2_596CCED255394A3A9081033D515D8153
