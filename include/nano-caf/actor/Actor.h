//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B
#define NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B

#include <nano-caf/msg/Message.h>
#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/actor/detail/ExpectMsgHandler.h>
#include <nano-caf/async/Promise.h>

namespace nano_caf {
    struct Actor {
        virtual ~Actor() = default;

    protected:
        template<typename T, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
        inline auto Send(ActorHandle const& to, ARGS&& ... args) const noexcept -> Status {
            return to.Send<T, CATEGORY>(static_cast<ActorHandle const&>(Self()), std::forward<ARGS>(args)...);
        }

        template<typename T, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
        inline auto Reply(ARGS&& ... args) const noexcept -> Status {
            return Send<T, CATEGORY>(CurrentSender(), std::forward<ARGS>(args)...);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::NORMAL, typename R = typename ATOM::Type::ResultType, typename ... ARGS>
        inline auto Request(ActorHandle const& to, ARGS&& ... args) const noexcept -> Future<R> {
            Promise<R> promise;
            auto status = to.DoRequest<typename ATOM::MsgType, CATEGORY>(static_cast<ActorHandle const&>(Self()), promise, std::forward<ARGS>(args)...);
            if(status != Status::OK) {
                return Promise<R>{status}.GetFuture();
            }
            return promise.GetFuture();
        }

        template<typename MSG, typename F, typename R = std::invoke_result_t<F, MSG>>
        auto ExpectMsg(F&& f) noexcept -> Future<R> {
            auto handler = new detail::ExpectMsgHandler<MSG>{};
            if(handler == nullptr) {
                return {};
            }

            auto&& future = handler->GetFuture();
            RegisterExpectOnceHandler(MSG::ID, handler);
            return future.Then(std::forward<F>(f));
        }

    protected:
        virtual auto Self() const noexcept -> ActorHandle = 0;
        virtual auto Exit(ExitReason) noexcept -> void = 0;

    private:
        virtual auto CurrentSender() const noexcept -> ActorHandle = 0;
        virtual auto RegisterExpectOnceHandler(MsgTypeId, detail::MsgHandler*) noexcept -> void = 0;
        virtual auto StartTimer(TimerSpec const& spec, bool periodic, TimeoutCallback&& callback) -> Result<TimerId> = 0;
    };
}

#endif //NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B
