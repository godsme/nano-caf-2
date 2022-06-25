//
// Created by Darwin Yuan on 2022/6/15.
//

#ifndef NANO_CAF_2_7CD95F8AA9C54F5BA8D719F757302C47
#define NANO_CAF_2_7CD95F8AA9C54F5BA8D719F757302C47

#include <memory>

namespace nano_caf {

    template <typename ELEM>
    struct Queue {
        Queue() = default;

        auto Enqueue(ELEM* elem) noexcept -> void;
        auto PushFront(ELEM* elem) noexcept -> void;
        auto Concat(Queue& another) noexcept -> void;
        auto Dequeue() noexcept -> ELEM*;
        auto TakeAll() noexcept -> ELEM*;

        auto Empty() const noexcept -> bool {
            return m_head == nullptr;
        }

        auto CleanUp() noexcept -> void;

        ~Queue() {
            CleanUp();
        }

    private:
        ELEM* m_head{};
        ELEM* m_tail{};
    };
}

#endif //NANO_CAF_2_7CD95F8AA9C54F5BA8D719F757302C47
