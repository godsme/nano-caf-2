//
// Created by Darwin Yuan on 2022/6/16.
//
#include <nano-caf/actor/LifoQueue.h>
#include <nano-caf/message/Message.h>
#include <memory>

namespace nano_caf {
    LifoQueue::LifoQueue() : m_stack{Blocked()} {}

    auto LifoQueue::Enqueue(Message* msg) noexcept -> Result {
        if(msg == nullptr) return Result::NULL_MSG;
        auto* head = m_stack.load();
        auto* closed = Eof();

        do {
            if(head == closed) {
                delete msg;
                return Result::CLOSED;
            }
            msg->m_next = head == Blocked() ? nullptr : head;
        } while(!m_stack.compare_exchange_weak(head, msg));

        return head == Blocked() ? Result::BLOCKED : Result::OK;
    }

    auto LifoQueue::TakeAll() noexcept -> Message* {
        auto* head = m_stack.load();
        auto* closed = Eof();

        do {
            if(head == nullptr || head == closed || head == Blocked()) {
                return nullptr;
            }
        } while(!m_stack.compare_exchange_weak(head, nullptr));

        return head;
    }

    auto LifoQueue::TryBlock() noexcept -> bool {
        Message* head = nullptr;
        if(m_stack.compare_exchange_strong(head, Blocked())) {
            return true;
        }
        return head == Blocked();
    }

    namespace {
        auto CleanUp(Message* msg) -> void {
            while(msg != nullptr) {
                std::unique_ptr<Message> m{msg};
                msg = msg->m_next;
            }
        }
    }

    auto LifoQueue::Close() noexcept -> void {
        auto* msg = m_stack.load();
        auto* closed = Eof();

        do {
            if(msg == closed) return;
        } while(!m_stack.compare_exchange_weak(msg, closed));

        if(msg == Blocked() || msg == nullptr) return;

        CleanUp(msg);
    }

    auto LifoQueue::IsClosed() const noexcept -> bool {
        return m_stack == Eof();
    }

    LifoQueue::~LifoQueue() {
        Close();
    }
}