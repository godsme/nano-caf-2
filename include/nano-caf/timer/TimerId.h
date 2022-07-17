//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_180785809842472AB4533845B39E05D2
#define NANO_CAF_2_180785809842472AB4533845B39E05D2

#include <nano-caf/timer/TimerSpec.h>
#include <chrono>
#include <cstdint>

namespace nano_caf {
    struct TimerId {
        TimerId() = default;
        TimerId(intptr_t, TimerSpec const&, std::chrono::steady_clock::time_point const&, std::size_t repeatTimes);
        TimerId(TimerId const&);
        TimerId(TimerId&&);
        ~TimerId();

        auto operator=(TimerId const&) -> TimerId&;
        auto operator=(TimerId&&) -> TimerId&;

        explicit operator bool() const {
            return m_desc != nullptr;
        }

        auto Cancel() -> void;
        auto OnExpire() -> bool;

        auto IsActive() const -> bool;
        auto IsCancelled() const -> bool;
        auto ShouldRepeat() const -> bool;

        auto GetActorId() const -> intptr_t;
        auto GetIssueTime() const -> std::chrono::steady_clock::time_point;
        auto GetTimeSpec() const -> TimerSpec const&;
        auto SetIssueTime(std::chrono::steady_clock::time_point) -> void;

        friend auto operator==(TimerId const& lhs, TimerId const& rhs) -> bool {
            return lhs.m_desc == rhs.m_desc;
        }

        friend auto operator!=(TimerId const& lhs, TimerId const& rhs) -> bool {
            return !operator==(lhs, rhs);
        }

    private:
        struct Descriptor;
        TimerId(Descriptor* desc);

    private:
        Descriptor* m_desc{};
    };
}

#endif //NANO_CAF_2_180785809842472AB4533845B39E05D2
