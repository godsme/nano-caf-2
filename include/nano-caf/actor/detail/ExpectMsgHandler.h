//
// Created by Darwin Yuan on 2022/6/24.
//

#ifndef NANO_CAF_2_6FBAB73673F84C89B2046F4F1AC84CDD
#define NANO_CAF_2_6FBAB73673F84C89B2046F4F1AC84CDD

#include <nano-caf/actor/detail/CancellableMsgHandler.h>
#include <nano-caf/async/detail/FutureObject.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/async/Future.h>
#include <memory>

namespace nano_caf::detail {
    template<typename MSG>
    struct ExpectMsgHandler : CancellableMsgHandler {
        using Object = std::shared_ptr<detail::FutureObject<MSG&>>;
        ExpectMsgHandler() : m_future{std::make_shared<detail::FutureObject<MSG&>>()} {}

        auto OnTimeout() noexcept -> bool {
            return m_future->OnTimeout();
        }

        auto Cancel() noexcept -> void override {
            m_future->Commit();
        }

        auto HandleMsg(Message& msg) noexcept -> bool override {
            if(msg.id != MSG::ID) return false;
            m_future->SetValue(msg.Body<MSG>());
            m_future->Commit();
            return true;
        }

        auto GetFuture() noexcept -> Object& {
            return m_future;
        }

    private:
        Object m_future;
    };
}

#endif //NANO_CAF_2_6FBAB73673F84C89B2046F4F1AC84CDD
