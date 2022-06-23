//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_22CF1C93F4C6438DA663C2DE4443A8E9
#define NANO_CAF_2_22CF1C93F4C6438DA663C2DE4443A8E9

#include <nano-caf/util/CallableTrait.h>
#include <nano-caf/msg/RequestConcept.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/msg/Atom.h>
#include <nano-caf/async/FutureConcept.h>
#include <nano-caf/async/AbstractPromise.h>
#include <utility>

namespace nano_caf::detail {
    template<typename F, typename MSG_TYPE>
    struct BehaviorBase {
        BehaviorBase(F &&f) : f_(std::move(f)) {}
        F f_;

        using ResultType = typename CallableTrait<F>::ResultType;

        auto HandleMsg(Message &msg, auto (*handler)(MSG_TYPE& msg, F& f) -> ResultType) -> bool {
            auto *body = msg.Body<MSG_TYPE>();
            if (body == nullptr) return false;
            if constexpr (IS_REQUEST<MSG_TYPE>) {
                auto* p = msg.Promise<MSG_TYPE>();
                if(p == nullptr) {
                    // should never happend
                    return true;
                }

                if constexpr (IS_FUTURE<ResultType>) {
                    p->Join(handler(*body, f_), msg.sender);
                } else if constexpr(std::is_void_v<ResultType>) {
                    handler(*body, f_);
                    p->Reply(std::move(Void::Instance()), msg.sender);
                } else {
                    p->Reply(handler(*body, f_), msg.sender);
                }
            } else {
                handler(*body, f_);
            }

            return true;
        }
    };
}

#endif //NANO_CAF_2_22CF1C93F4C6438DA663C2DE4443A8E9
