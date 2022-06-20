//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_EFFC0EC0AAFA4589B1CEC860CF2B8979
#define NANO_CAF_2_EFFC0EC0AAFA4589B1CEC860CF2B8979

#include <nano-caf/actor/LifoQueue.h>
#include <nano-caf/util/Queue.h>
#include <nano-caf/scheduler/TaskResult.h>
#include <functional>

namespace nano_caf {
    struct Message;

    struct MailBox : private LifoQueue {
        using Consumer = std::function<auto (Message&) -> TaskResult>;
        using Result = LifoQueue::Result;

        MailBox() = default;

        auto SendMsg(Message* msg) noexcept -> LifoQueue::Result {
            return LifoQueue::Enqueue(msg);
        }

        auto Consume(std::size_t quota, Consumer) -> TaskResult;

        auto Empty() const -> bool {
            return m_normal.Empty() && m_urgent.Empty();
        }

        auto Close() noexcept -> void;

    private:
        auto Reload() noexcept -> bool;
        auto ReloadOne(Message* msg) noexcept -> void;
        auto ReloadMany(Message*) noexcept -> void;

    private:
        Queue<Message> m_normal{};
        Queue<Message> m_urgent{};
    };
}

#endif //NANO_CAF_2_EFFC0EC0AAFA4589B1CEC860CF2B8979
