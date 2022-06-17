//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/MailBox.h>

namespace nano_caf {
    auto MailBox::ReloadOne(Message* msg) noexcept -> void {
        if(msg->m_category == Message::Category::URGENT) {
            m_urgent.Enqueue(msg);
        } else {
            m_normal.Enqueue(msg);
        }
    }

    auto MailBox::ReloadMany(Message* msg) noexcept -> void {
        Queue<Message> normal;
        Queue<Message> urgent;

        do {
            if(msg->m_category == Message::Category::URGENT) {
                urgent.Enqueue(msg);
            } else {
                normal.Enqueue(msg);
            }
            msg = msg->m_next;
        } while(msg != nullptr);

        m_urgent.Concat(urgent);
        m_normal.Concat(normal);
    }

    auto MailBox::Reload() noexcept -> bool {
        auto* msg = LifoQueue::TakeAll();
        if(msg == nullptr) return false;

        if(msg->m_next == nullptr) {
            // only 1 message in the LIFO queue
            ReloadOne(msg);
        } else {
            ReloadMany(msg);
        }

        return true;
    }

    namespace {
        auto Process(Queue<Message>& queue, MailBox::Consumer& f) -> TaskResult {
            while(1) {
                auto msg = std::unique_ptr<Message>(queue.Dequeue());
                if(msg == nullptr) return TaskResult::RESUME;
                if(f(*msg) == TaskResult::DONE) {
                    return TaskResult::DONE;
                }
            }
        }
    }

    auto MailBox::Consume(Consumer f) -> void {
        while(1) {
            while(!Reload()) {
                if(LifoQueue::IsClosed()) return;
                if(LifoQueue::TryBlock()) return;
            }

            if(Process(m_urgent, f) == TaskResult::DONE) {
                break;
            }

            if(Process(m_normal, f) == TaskResult::DONE) {
                break;
            }
        }

        LifoQueue::Close();
    }

    auto MailBox::Close() noexcept -> void {
        LifoQueue::Close();
        m_urgent.CleanUp();
        m_normal.CleanUp();
    }
}