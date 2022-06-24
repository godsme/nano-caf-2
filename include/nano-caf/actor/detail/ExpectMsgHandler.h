//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_6FBAB73673F84C89B2046F4F1AC84CDD
#define NANO_CAF_2_6FBAB73673F84C89B2046F4F1AC84CDD

#include <nano-caf/actor/detail/MsgHandler.h>
#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/async/Future.h>
#include <memory>

namespace nano_caf::detail {
    template<typename MSG>
    struct ExpectMsgHandler : MsgHandler {
        ExpectMsgHandler() : m_future{std::make_shared<detail::FutureObject<MSG&>>()} {}

        auto HandleMsg(Message& msg) noexcept -> bool {
            if(msg.id != MSG::ID) return false;
            m_future->SetValue(msg.Body<MSG>());
            m_future->Commit();
            return true;
        }

        auto GetFuture() noexcept -> auto {
            return Future<MSG&>{m_future};
        }

    private:
        std::shared_ptr<detail::FutureObject<MSG&>> m_future;
    };
}

#endif //NANO_CAF_2_6FBAB73673F84C89B2046F4F1AC84CDD
