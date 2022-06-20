//
// Created by Darwin Yuan on 2022/6/15.
//

#include <nano-caf/scheduler/WorkSharingQueue.h>

namespace nano_caf {
    auto WorkSharingQueue::Reschedule(Resumable* task) noexcept -> void {
        {
            std::unique_lock lock{m_lock};
            m_tasks.Enqueue(task);
        }
        m_cv.notify_one();
    }

    auto WorkSharingQueue::Enqueue(Resumable* task) noexcept -> void {
        task->AddRef();
        Reschedule(task);
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

    auto WorkSharingQueue::CleanUp() noexcept -> void {
        std::unique_lock lock{m_lock};
        while(1) {
            auto* task = m_tasks.Dequeue();
            if(task == nullptr) break;
            task->Release();
        }
    }

    auto WorkSharingQueue::IsEmpty() const noexcept -> bool {
        std::unique_lock lock{m_lock};
        return m_tasks.Empty();
    }

    WorkSharingQueue::~WorkSharingQueue() {
        CleanUp();
    }
}