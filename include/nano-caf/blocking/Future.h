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
        auto Then(F&& cb) -> void {
            if(m_object == nullptr) {
                cb(Result<R>(ResultTag::CAUSE, Status::NULL_PTR));
            } else {
                m_object->SetCallback(cb);
            }
        }

        auto Get() noexcept -> Result<R> {
            CvNotifier cv;
            std::optional<Result<R>> value;
            Then([&](auto&& result) {
                cv.WakeUp([&] {
                    value.emplace(static_cast<decltype(result)>(result));
                });
            });
            cv.Wait([&]{ return value.has_value(); });
            return *value;
        }

    private:
        Object m_object{};
    };
}

#endif //NANO_CAF_2_08CA3EC9F076472892CE9F63AFF10533
