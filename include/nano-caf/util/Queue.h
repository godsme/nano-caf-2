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

        auto Enqueue(ELEM* elem) noexcept -> void {
            if(elem == nullptr) return;
            if(m_tail != nullptr) m_tail->m_next = elem;
            else m_head = elem;
            m_tail = elem;
            elem->m_next = nullptr;
        }

        auto PushFront(ELEM* elem) noexcept -> void {
            if(elem == nullptr) return;
            elem->m_next = m_head;
            m_head = elem;
            if(m_tail == nullptr) m_tail = elem;
        }

        auto Concat(Queue& another) noexcept -> void {
            if(another.Empty()) return;
            if(Empty()) {
                std::swap(m_head, another.m_head);
                std::swap(m_tail, another.m_tail);
            } else {
                m_tail->m_next = another.m_head;
                m_tail = another.m_tail;
                another.m_head = nullptr;
                another.m_tail = nullptr;
            }
        }

        auto Dequeue() noexcept -> ELEM* {
            if(m_head == nullptr) return nullptr;
            auto elem = m_head;
            m_head = elem->m_next;
            if (m_head == nullptr) m_tail = nullptr;
            return elem;
        }

        auto Empty() const noexcept -> bool {
            return m_head == nullptr;
        }

        auto CleanUp() noexcept -> void {
            while(m_head != nullptr) {
                std::unique_ptr<ELEM> ptr{m_head};
                m_head = m_head->m_next;
            }
            m_tail = nullptr;
        }

        ~Queue() {
            CleanUp();
        }

    private:
        ELEM* m_head{};
        ELEM* m_tail{};
    };
}

#endif //NANO_CAF_2_7CD95F8AA9C54F5BA8D719F757302C47
