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

    protected:
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

        WeakPtr(WeakPtr const& another) : WeakPtr{another.m_ptr} {}
        WeakPtr(WeakPtr&& another) : Parent{another.m_ptr} {
            another.m_ptr = nullptr;
        }

        auto operator=(WeakPtr const& another) noexcept -> WeakPtr& {
            Reset();
            m_ptr = another.m_ptr;
            if(m_ptr != nullptr) {
                CtlBlock()->AddWeakRef();
            }

            return *this;
        }

        auto operator=(WeakPtr&& another) noexcept -> WeakPtr& {
            std::swap(m_ptr, another.m_ptr);
            return *this;
        }

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

        friend inline auto operator==(WeakPtr<T> const& lhs, WeakPtr<T> const& rhs) noexcept -> bool {
            return lhs.m_ptr == rhs.m_ptr;
        }

        friend inline auto operator==(WeakPtr<T> const& lhs, std::nullptr_t) noexcept -> bool {
            return lhs.m_ptr == nullptr;
        }

        friend inline auto operator!=(WeakPtr<T> const& lhs, std::nullptr_t) noexcept -> bool {
            return lhs.m_ptr != nullptr;
        }

        friend inline auto operator!=(WeakPtr<T> const& lhs, WeakPtr<T> const& rhs) noexcept -> bool {
            return !operator==(lhs, rhs);
        }

        friend inline auto operator==(std::nullptr_t, WeakPtr<T> const& rhs) noexcept -> bool {
            return nullptr == rhs.m_ptr;
        }

        friend inline auto operator!=(std::nullptr_t, WeakPtr<T> const& rhs) noexcept -> bool {
            return nullptr != rhs.m_ptr;
        }
    };
}

#endif //NANO_CAF_2_E373A7EC8C3847A6A99AD20BCEE36FE6
