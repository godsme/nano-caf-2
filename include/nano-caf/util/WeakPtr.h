//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_E373A7EC8C3847A6A99AD20BCEE36FE6
#define NANO_CAF_2_E373A7EC8C3847A6A99AD20BCEE36FE6

#include <nano-caf/util/SharedPtr.h>

namespace nano_caf {
    template <typename T>
    struct WeakPtr : SharedPtrDataHolder<T> {
    private:
        using Parent = SharedPtrDataHolder<T>;
        using Parent::m_ptr;
        using Parent::CtlBlock;

    public:
        WeakPtr() noexcept = default;
        explicit WeakPtr(T* ptr) noexcept : Parent{ptr} {
            if(ptr != nullptr) {
                CtlBlock()->AddWeakRef();
            }
        }

        explicit WeakPtr(SharedPtr<T> const& shared) noexcept : WeakPtr{shared.Get()} {}
        explicit WeakPtr(SharedPtr<T>&& shared) noexcept : WeakPtr{shared.Get()} {}

        auto Lock() const noexcept -> SharedPtr<T> {
            if(m_ptr == nullptr || !CtlBlock()->Lock()) {
                return {};
            } else {
                return SharedPtr<T>{m_ptr, false};
            }
        }

        auto UseCount() const noexcept -> std::size_t {
            return m_ptr == nullptr ? 0 : CtlBlock()->UseCount();
        }

        auto Expired() const noexcept -> bool {
            return UseCount() == 0;
        }

        auto Reset() noexcept -> void {
            if(m_ptr == nullptr) return;
            CtlBlock()->ReleaseWeakRef();
            m_ptr = nullptr;
        }

        ~WeakPtr() {
            Reset();
        }
    };
}

#endif //NANO_CAF_2_E373A7EC8C3847A6A99AD20BCEE36FE6
