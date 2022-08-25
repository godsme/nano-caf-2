//
// Created by Darwin Yuan on 2022/8/20.
//

#ifndef NANO_CAF_2_9BF73CAC6E0A45F4A1EA9F6C2FDD7751
#define NANO_CAF_2_9BF73CAC6E0A45F4A1EA9F6C2FDD7751

#include <nano-caf/msg/MakeMessage.h>
#include <nano-caf/msg/Atom.h>
#include <nano-caf/blocking/BlockingPromise.h>

namespace nano_caf {
    struct GlobalActorContext {
        template<typename MSG, Message::Category CATEGORY = Message::DEFAULT, typename ... ARGS>
        static auto Send(ActorPtr const& to, ARGS&& ... args) noexcept -> Status {
            return to.SendMsg(MakeMessage<MsgType<MSG>, CATEGORY>(std::forward<ARGS>(args)...));
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename R = typename ATOM::Type::ResultType, typename ... ARGS>
        static auto Async(ActorPtr const& to, ARGS&& ... args) -> BlockingFuture<R> {
            BlockingPromise<R> promise{};
            auto&& future = promise.GetFuture();
            CAF_ASSERT_TRUE_R(future, {Status::NULL_PTR});
            CAF_ASSERT_R(to.SendMsg(MakeRequest<typename ATOM::MsgType, CATEGORY>(std::move(promise), std::forward<ARGS>(args)...)), {status_});
            return std::move(future);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename ... ARGS>
        static auto Request(ActorPtr const& to, ARGS&& ... args) -> auto {
            return DoRequest<ATOM, CATEGORY>(to, [](auto&& future) {
                return future.Wait();
            }, std::forward<ARGS>(args)...);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename Rep, typename Period, typename ... ARGS>
        static auto Request(ActorPtr const& to, std::chrono::duration<Rep, Period> duration, ARGS&& ... args) -> auto {
            return DoRequest<ATOM, CATEGORY>(to, [&duration](auto&& future) {
                return future.WaitFor(duration);
            }, std::forward<ARGS>(args)...);
        }

    private:
        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename R = typename ATOM::Type::ResultType, typename F, typename ... ARGS>
        static auto DoRequest(ActorPtr const& to, F&& f, ARGS&& ... args) -> Result<R> {
            return f(Async<ATOM, CATEGORY>(to, std::forward<ARGS>(args)...));
        }
    };
}

#endif //NANO_CAF_2_9BF73CAC6E0A45F4A1EA9F6C2FDD7751
