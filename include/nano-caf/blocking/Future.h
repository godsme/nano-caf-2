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
        using ResultType = ValueTypeOf<R>;
        using Object = std::shared_ptr<detail::FutureObject<R>>;

        Future() : m_object{Status::NULL_PTR} {}
        Future(Status cause) : m_object{cause} {}
        Future(Object object) noexcept : m_object{std::move(object)} {
            if(*m_object == nullptr) {
                m_object = Result<Object>{Status::NULL_PTR};
            }
        }

        explicit operator bool () {
            return m_object.Ok();
        }

        template<typename F>
        auto Then(F&& cb) -> void {
            if(!m_object.Ok()) {
                cb(Result<R>(ResultTag::CAUSE, m_object.GetStatus()));
            } else if(auto status = (*m_object)->SetCallback(cb); status != Status::OK) {
                cb(Result<R>(ResultTag::CAUSE, status));
            }
        }

        auto Wait() noexcept -> Result<R> {
            CvNotifier cv;
            std::optional<Result<R>> value;
            Then([&](auto&& result) {
                cv.WakeUp([&] { value.emplace(static_cast<decltype(result)>(result)); });
            });
            cv.Wait([&]{ return value.has_value(); });
            return *value;
        }

        template<typename Rep, typename Period>
        auto WaitFor(std::chrono::duration<Rep, Period> duration) noexcept -> Result<R> {
            CvNotifier cv;
            std::optional<Result<R>> value;
            Then([&](auto&& result) {
                cv.WakeUp([&] { value.emplace(static_cast<decltype(result)>(result)); });
            });
            if(!cv.WaitFor(duration, [&]{ return value.has_value(); })) {
                if(m_object.Ok()) {
                    m_object.GetValue()->UnsetCallback();
                }
            }
            return value ? *value : Result<R>{ResultTag::CAUSE, Status::TIMEOUT};
        }

    private:
        Result<Object> m_object;
    };
}

#endif //NANO_CAF_2_08CA3EC9F076472892CE9F63AFF10533
