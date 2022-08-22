//
// Created by Darwin Yuan on 2022/8/22.
//

#ifndef NANO_CAF_2_08CA3EC9F076472892CE9F63AFF10533
#define NANO_CAF_2_08CA3EC9F076472892CE9F63AFF10533

#include <nano-caf/blocking/detail/FutureObject.h>
#include <nano-caf/util/Assertions.h>
#include <nano-caf/util/CvNotifier.h>
#include <nano-caf/util/Result.h>
#include <memory>

namespace nano_caf::blocking {
    template<typename R>
    struct Future {
        using Object = std::shared_ptr<detail::FutureObject<R>>;
        Future() = default;
        Future(Object const& object) noexcept : m_object{object} {}

        template<typename F>
        auto SetCallback(F&& cb) -> Status {
            if(m_object != nullptr) {
                return m_object->SetCallback(cb);
            }
            cb(Result<R>(ResultTag::CAUSE, Status::NULL_PTR));
            return Status::OK;
        }

        auto Wait() noexcept -> Result<R> {
            CvNotifier cv;
            std::optional<Result<R>> value;
            auto status = SetCallback([&](auto&& result) {
                cv.WakeUp([&] {
                    value.emplace(static_cast<decltype(result)>(result));
                });
            });
            if(status != Status::OK) {
                return {ResultTag::CAUSE, status};
            }
            cv.Wait([&]{ return value.has_value(); });
            return *value;
        }

        template<typename Rep, typename Period>
        auto WaitFor(std::chrono::duration<Rep, Period> duration) noexcept -> Result<R> {
            CvNotifier cv;
            std::optional<Result<R>> value;
            auto status = SetCallback([&](auto&& result) {
                cv.WakeUp([&] {
                    value.emplace(static_cast<decltype(result)>(result));
                });
            });
            if(status != Status::OK) {
                return {ResultTag::CAUSE, status};
            }
            if(cv.WaitFor(duration, [&]{ return value.has_value(); })) {
                return *value;
            }

            m_object->UnsetCallback();
            if(value) {
                return *value;
            }

            return {ResultTag::CAUSE, Status::TIMEOUT};
        }

    private:
        Object m_object{};
    };
}

#endif //NANO_CAF_2_08CA3EC9F076472892CE9F63AFF10533
