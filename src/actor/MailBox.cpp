//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/MailBox.h>
#include <nano-caf/message/Message.h>
#include <nano-caf/util/Queue.tcc>

namespace nano_caf {
    template struct Queue<Message>;

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
            auto* m = msg;
            msg = msg->m_next;
            if(m->m_category == Message::Category::URGENT) {
                urgent.PushFront(m);
            } else {
                normal.PushFront(m);
            }
        } while(msg != nullptr);

        m_urgent.Concat(urgent);
        m_normal.Concat(normal);
    }

    auto MailBox::Reload() noexcept -> bool {
        auto* msg = LifoQueue::TakeAll();
        if(msg == nullptr) return !Empty();
        if(msg->m_next == nullptr) {
            // only 1 msg in the LIFO queue
            ReloadOne(msg);
        } else {
            ReloadMany(msg);
        }

        return true;
    }

    namespace {
        auto Process(Queue<Message>& queue, std::size_t& remain, MailBox::Consumer& f) -> TaskResult {
            while(remain > 0) {
                auto msg = std::unique_ptr<Message>(queue.Dequeue());
                if(msg == nullptr) return TaskResult::SUSPENDED;
                if(f(*msg) == TaskResult::DONE) {
                    return TaskResult::DONE;
                }
                --remain;
            }

            return TaskResult::SUSPENDED;
        }
    }

    auto MailBox::Consume(std::size_t quota, Consumer f) -> TaskResult {
        std::size_t remain = quota;

        while(1) {
            while(!Reload()) {
                if(LifoQueue::IsClosed()) return TaskResult::DONE;
                if(LifoQueue::TryBlock()) return TaskResult::DONE;
            }

            // There must be some unconsumed messages in mailbox.
            if(remain == 0) return TaskResult::SUSPENDED;

            if(Process(m_urgent, remain, f) == TaskResult::DONE) {
                break;
            }

            if(remain == 0) continue;

            if(Process(m_normal, remain, f) == TaskResult::DONE) {
                break;
            }
        }

        Close();

        return TaskResult::DONE;
    }

    auto MailBox::Close() noexcept -> void {
        LifoQueue::Close();
        m_urgent.CleanUp();
        m_normal.CleanUp();
    }
}