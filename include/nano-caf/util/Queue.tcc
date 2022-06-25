//
// Created by Darwin Yuan on 2022/6/21.
//
#include <nano-caf/util/Queue.h>

namespace nano_caf {

    template <typename ELEM>
    auto Queue<ELEM>::Enqueue(ELEM* elem) noexcept -> void {
        if(elem == nullptr) return;
        if(m_tail != nullptr) m_tail->m_next = elem;
        else m_head = elem;
        m_tail = elem;
        elem->m_next = nullptr;
    }

    template <typename ELEM>
    auto Queue<ELEM>::PushFront(ELEM* elem) noexcept -> void {
        if(elem == nullptr) return;
        elem->m_next = m_head;
        m_head = elem;
        if(m_tail == nullptr) m_tail = elem;
    }

    template <typename ELEM>
    auto Queue<ELEM>::Concat(Queue& another) noexcept -> void {
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

    template <typename ELEM>
    auto Queue<ELEM>::Dequeue() noexcept -> ELEM* {
        if(m_head == nullptr) return nullptr;
        auto elem = m_head;
        m_head = elem->m_next;
        if (m_head == nullptr) m_tail = nullptr;
        return elem;
    }

    template <typename ELEM>
    auto Queue<ELEM>::CleanUp() noexcept -> void {
        m_tail = nullptr;
        while(m_head != nullptr) {
            std::unique_ptr<ELEM> ptr{m_head};
            m_head = m_head->m_next;
        }
    }

    template <typename ELEM>
    auto Queue<ELEM>::CleanUp(Cleaner& cleaner) noexcept -> void {
        m_tail = nullptr;
        while(m_head != nullptr) {
            std::unique_ptr<ELEM> ptr{m_head};
            cleaner(*ptr);
            m_head = m_head->m_next;
        }
    }

    template<typename ELEM>
    auto Queue<ELEM>::TakeAll() noexcept -> ELEM * {
        auto* result = m_head;
        m_head = nullptr;
        m_tail = nullptr;
        return result;
    }
}