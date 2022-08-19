//
// Created by Darwin Yuan on 2022/8/20.
//

#ifndef NANO_CAF_2_9BF73CAC6E0A45F4A1EA9F6C2FDD7751
#define NANO_CAF_2_9BF73CAC6E0A45F4A1EA9F6C2FDD7751

#include <nano-caf/msg/MakeMessage.h>
#include <nano-caf/msg/Atom.h>
#include <nano-caf/async/Requester.h>

namespace nano_caf {
    struct GlobalActorContext {
        template<typename MSG, Message::Category CATEGORY = Message::DEFAULT, std::enable_if_t<!std::is_base_of_v<nano_caf::AtomSignature, MSG>, bool> = true, typename ... ARGS>
        static auto Send(ActorPtr const& to, ARGS&& ... args) noexcept -> Status {
            return to.SendMsg(MakeMessage<MSG, CATEGORY>(std::forward<ARGS>(args)...));
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, std::enable_if_t<std::is_base_of_v<nano_caf::AtomSignature, ATOM>, bool> = true, typename ... ARGS>
        static auto Send(ActorPtr const& to, ARGS&& ... args) noexcept -> Status {
            return Send<typename ATOM::MsgType, CATEGORY>(to, std::forward<ARGS>(args)...);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename R = typename ATOM::Type::ResultType, typename ... ARGS>
        static auto Request(ActorPtr const& to, ARGS&& ... args) -> Result<R> {
            return DoRequest<ATOM, R, CATEGORY>(to, [](auto&& future) -> Result<R> {
                return future.get();
            }, std::forward<ARGS>(args)...);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename R = typename ATOM::Type::ResultType, typename Rep, typename Period, typename ... ARGS>
        static auto Request(ActorPtr const& to, std::chrono::duration<Rep, Period> duration, ARGS&& ... args) -> Result<R> {
            return DoRequest<ATOM, R, CATEGORY>(to, [&duration](auto&& future) {
                if(future.wait_for(duration) != std::future_status::ready) {
                    return Result<R>{ResultTag::CAUSE, Status::TIMEOUT};
                }
                return future.get();
            }, std::forward<ARGS>(args)...);
        }

    private:
        template<typename ATOM, typename R, Message::Category CATEGORY = Message::DEFAULT, typename F, typename ... ARGS>
        static auto DoRequest(ActorPtr const& to, F&& f, ARGS&& ... args) -> Result<R> {
            Requester<R> requester{};
            auto&& future = requester.GetFuture();
            auto status = to.SendMsg(MakeRequest<typename ATOM::MsgType, CATEGORY>(std::move(requester), std::forward<ARGS>(args)...));
            if(status != Status::OK) {
                return Result<R>{ResultTag::CAUSE, status};
            }
            return f(future);
        }
    };
}

#endif //NANO_CAF_2_9BF73CAC6E0A45F4A1EA9F6C2FDD7751
