//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/MailBox.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/util/Queue.tcc>

namespace nano_caf {
    template struct Queue<Message>;

    auto MailBox::ReloadOne(Message* msg) noexcept -> void {
        if(msg->category == Message::Category::URGENT) {
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
            if(m->category == Message::Category::URGENT) {
                urgent.PushFront(m);
            } else {
                normal.PushFront(m);
            }
        } while(msg != nullptr);

        m_urgent.Concat(urgent);
        m_normal.Concat(normal);
    }

    auto MailBox::Put(Message* msg) noexcept -> bool {
        if(msg == nullptr) {
            return false;
        }

        if(msg->m_next == nullptr) {
            // only 1 msg in the LIFO queue
            ReloadOne(msg);
        } else {
            ReloadMany(msg);
        }
        return true;
    }

    auto MailBox::Reload() noexcept -> bool {
        return Put(LifoQueue::TakeAll());
    }

    namespace {
        auto Process(Queue<Message>& queue, std::size_t& remain, MailBox::Consumer& f) -> TaskResult {
            while(remain > 0) {
                auto* msg = queue.Dequeue();
                if(msg == nullptr) return TaskResult::SUSPENDED;
                if(f(std::unique_ptr<Message>(msg)) == TaskResult::DONE) {
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
            while(!Reload() && Empty()) {
                if(LifoQueue::IsClosed() || LifoQueue::TryBlock()) return TaskResult::DONE;
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

    auto MailBox::MsgQueue::Clear() noexcept -> void {
        CleanUp([] (Message& msg) { msg.OnDiscard(); });
    }

    MailBox::MsgQueue::~MsgQueue() {
        Clear();
    }

    auto MailBox::Close() noexcept -> void {
        LifoQueue::Close();
        m_urgent.Clear();
        m_normal.CleanUp();
    }
}