//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/WorkSharingQueue.h>

namespace nano_caf {
    auto WorkSharingQueue::Enqueue(Resumable* task) noexcept -> void {
        if(task == nullptr) return;
        {
            std::unique_lock lock{m_lock};
            m_tasks.Enqueue(task);
            task->AddRef();
        }
        m_cv.notify_one();
    }

    auto WorkSharingQueue::Dequeue() noexcept -> Resumable * {
        std::unique_lock lock{m_lock};
        m_cv.wait(lock, [this]{ return !m_tasks.Empty() || m_shutdown; });
        if(m_shutdown) return nullptr;
        return m_tasks.Dequeue();
    }

    auto WorkSharingQueue::Shutdown() noexcept -> void {
        {
            std::unique_lock lock{m_lock};
            if(m_shutdown) return;
            m_shutdown = true;
        }
        m_cv.notify_all();
    }
}