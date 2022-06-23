//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_48DBBF88487C4E3FB1160B394961DF5D
#define NANO_CAF_2_48DBBF88487C4E3FB1160B394961DF5D

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
                if(p == nullptr) return true;
                auto sender = msg.sender.Lock();
                if(!sender) return true;
                if constexpr (IS_FUTURE<ResultType>) {
                    p->Join(handler(*body, f_), std::move(msg.sender));
                } else {
                    p->Reply(handler(*body, f_), std::move(sender));
                }
            } else {
                handler(*body, f_);
            }
            return true;
        }
    };
}

#endif //NANO_CAF_2_48DBBF88487C4E3FB1160B394961DF5D
