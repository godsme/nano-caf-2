//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_F33F3D00ABB14F28925898A8B4AD46E8
#define NANO_CAF_2_F33F3D00ABB14F28925898A8B4AD46E8

#include <nano-caf/Status.h>
#include <atomic>

namespace nano_caf {
    struct Message;

    struct LifoQueue {
        LifoQueue();
        ~LifoQueue();

        auto Enqueue(Message*) noexcept -> Status;
        auto TakeAll() noexcept -> Message*;
        auto TryBlock() noexcept -> bool;

        auto IsClosed() const noexcept -> bool;
        auto IsBlocked() const noexcept -> bool;

        auto Close() noexcept -> void;

    private:
        auto Eof() const noexcept -> Message * {
            return const_cast<Message*>(reinterpret_cast<Message const*>(m_closeTag));
        }

        auto Blocked() const noexcept -> Message* {
            return const_cast<Message*>(reinterpret_cast<Message const*>(m_blockTag));
        }
    private:
        char m_blockTag[0];
        std::atomic<Message*> m_stack{};
        char m_closeTag[0];
    };
}

#endif //NANO_CAF_2_F33F3D00ABB14F28925898A8B4AD46E8
