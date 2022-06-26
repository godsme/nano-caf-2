//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_7FC4950D7AED42E3BBAE560EFD95AFED
#define NANO_CAF_2_7FC4950D7AED42E3BBAE560EFD95AFED

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace nano_caf {
    struct CvNotifier {
        template<typename F>
        auto Wait(F&& f) noexcept -> void {
            std::unique_lock lock(m_mutex);
            m_sleeping = true;
            m_cv.wait(lock, std::forward<F>(f));
            m_sleeping = false;
        }

        auto Wait() noexcept -> void {
            std::unique_lock lock(m_mutex);
            m_sleeping = true;
            m_cv.wait(lock);
            m_sleeping = false;
        }

        template<class Rep, class Period, typename F>
        auto WaitFor(std::chrono::duration<Rep, Period> const& duration, F&& f) noexcept -> bool {
            std::unique_lock lock(m_mutex);
            m_sleeping = true;
            auto result = m_cv.wait_for(lock, duration, std::forward<F>(f));
            m_sleeping = false;
            return result;
        }

        template<class Clock, class Duration, typename F>
        auto WaitUntil(std::chrono::time_point<Clock, Duration> const& point, F&& f) noexcept -> bool {
            std::unique_lock lock(m_mutex);
            m_sleeping = true;
            auto result = m_cv.wait_until(lock, point, std::forward<F>(f));
            m_sleeping = false;
            return result;
        }

        template<class Clock, class Duration>
        auto WaitUntil(std::chrono::time_point<Clock, Duration> const& point) noexcept -> bool {
            std::unique_lock lock(m_mutex);
            m_sleeping = true;
            auto result = m_cv.wait_until(lock, point);
            m_sleeping = false;
            return result;
        }

        auto WakeUp() -> void {
            std::unique_lock lock(m_mutex);
            if(m_sleeping) {
                m_cv.notify_one();
            }
        }

    private:
        std::mutex m_mutex{};
        std::condition_variable m_cv{};
        bool m_sleeping{false};
    };
}

#endif //NANO_CAF_2_7FC4950D7AED42E3BBAE560EFD95AFED
