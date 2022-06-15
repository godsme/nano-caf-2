//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_58494686043E4599A99933519E5129AC
#define NANO_CAF_2_58494686043E4599A99933519E5129AC

#include <memory>

namespace nano_caf {

    template <typename ELEM>
    struct queue {
        queue() = default;

        auto enqueue(ELEM* elem) noexcept -> void {
            if(elem == nullptr) return;
            if(tail_ != nullptr) tail_->next = elem;
            else head_ = elem;
            tail_ = elem;
            elem->next = nullptr;
        }

        auto dequeue() noexcept -> ELEM* {
            if(head_ == nullptr) return nullptr;
            auto elem = head_;
            head_ = elem->next;
            if (head_ == nullptr) tail_ = nullptr;
            return elem;
        }

        auto empty() const noexcept -> bool {
            return head_ == nullptr;
        }

        ~queue() {
            auto* elem = head_;
            while(elem != nullptr) {
                std::unique_ptr<ELEM> ptr{elem};
                elem = elem->next_;
            }
        }

    private:
        ELEM* head_{};
        ELEM* tail_{};
    };
}

#endif //NANO_CAF_2_58494686043E4599A99933519E5129AC
