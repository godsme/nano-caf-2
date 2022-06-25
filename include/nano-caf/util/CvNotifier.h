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
            std::unique_lock lock(mutex_);
            sleeping_ = true;
            cv_.wait(lock, std::forward<F>(f));
            sleeping_ = false;
        }

        auto Wait() noexcept -> void {
            std::unique_lock lock(mutex_);
            sleeping_ = true;
            cv_.wait(lock);
            sleeping_ = false;
        }

        template<class Rep, class Period, typename F>
        auto WaitFor(std::chrono::duration<Rep, Period> const& duration, F&& f) noexcept -> bool {
            std::unique_lock lock(mutex_);
            sleeping_ = true;
            auto result = cv_.wait_for(lock, duration, std::forward<F>(f));
            sleeping_ = false;
            return result;
        }

        template<class Clock, class Duration, typename F>
        auto WaitUntil(std::chrono::time_point<Clock, Duration> const& point, F&& f) noexcept -> bool {
            std::unique_lock lock(mutex_);
            sleeping_ = true;
            auto result = cv_.wait_until(lock, point, std::forward<F>(f));
            sleeping_ = false;
            return result;
        }

        template<class Clock, class Duration>
        auto WaitUntil(std::chrono::time_point<Clock, Duration> const& point) noexcept -> bool {
            std::unique_lock lock(mutex_);
            sleeping_ = true;
            auto result = cv_.wait_until(lock, point);
            sleeping_ = false;
            return result;
        }

        auto WakeUp() -> void {
            std::unique_lock lock(mutex_);
            if(sleeping_) {
                cv_.notify_one();
            }
        }

    private:
        std::mutex mutex_{};
        std::condition_variable cv_{};
        bool sleeping_{false};
    };
}

#endif //NANO_CAF_2_7FC4950D7AED42E3BBAE560EFD95AFED
