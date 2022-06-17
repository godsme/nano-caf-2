//
// Created by Darwin Yuan on 2022/6/17.
//

#ifndef NANO_CAF_2_EFFC0EC0AAFA4589B1CEC860CF2B8979
#define NANO_CAF_2_EFFC0EC0AAFA4589B1CEC860CF2B8979

#include <nano-caf/actor/LifoQueue.h>
#include <nano-caf/util/Queue.h>
#include <nano-caf/message/Message.h>
#include <functional>

namespace nano_caf {
    enum class TaskResult {
        RESUME,
        DONE
    };

    struct MailBox : private LifoQueue {
        using Consumer = std::function<auto (Message&) -> TaskResult>;

        MailBox() = default;

        using LifoQueue::Enqueue;

        auto Consume(Consumer consumer) -> void;

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
