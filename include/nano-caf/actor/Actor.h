//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B
#define NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B

#include <nano-caf/actor/detail/ExpectMsgHandler.h>
#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/actor/Behavior.h>
#include <nano-caf/async/Promise.h>
#include <nano-caf/Status.h>

namespace nano_caf {
    struct Actor {
        virtual ~Actor() = default;

    private:
        template<typename Rep, typename Period>
        static inline auto InMs(std::chrono::duration<Rep, Period> timeout) -> Duration {
            return std::chrono::microseconds(timeout).count();
        }

    public:
        virtual auto Self() const noexcept -> ActorPtr = 0;
        virtual auto Exit(ExitReason) noexcept -> void = 0;
        virtual auto ChangeBehavior(Behavior const& to) noexcept -> void = 0;

        template<typename MSG, Message::Category CATEGORY = Message::DEFAULT, std::enable_if_t<!std::is_base_of_v<nano_caf::AtomSignature, MSG>, bool> = true, typename ... ARGS>
        inline auto Send(ActorPtr const& to, ARGS&& ... args) const noexcept -> Status {
            return to.SendMsg(MakeMessage<MSG, CATEGORY>(static_cast<ActorPtr const&>(Self()), std::forward<ARGS>(args)...));
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, std::enable_if_t<std::is_base_of_v<nano_caf::AtomSignature, ATOM>, bool> = true, typename ... ARGS>
        static auto Send(ActorPtr const& to, ARGS&& ... args) noexcept -> Status {
            return Send<typename ATOM::MsgType, CATEGORY>(to, std::forward<ARGS>(args)...);
        }

        template<typename MSG, Message::Category CATEGORY = Message::DEFAULT, typename ... ARGS>
        inline auto ToSelf(ARGS&& ... args) const noexcept -> Status {
            return Self().SendMsg(MakeMessage<MSG, CATEGORY>(std::forward<ARGS>(args)...));
        }

        template<typename T, Message::Category CATEGORY = Message::DEFAULT, typename ... ARGS>
        inline auto Reply(ARGS&& ... args) const noexcept -> Status {
            return Send<T, CATEGORY>(CurrentSender(), std::forward<ARGS>(args)...);
        }

        template<typename R, Message::Category CATEGORY = Message::DEFAULT>
        inline auto ForwardTo(ActorPtr const& to) const noexcept -> Future<R> {
            return Future<R>::Forward(ForwardTo(to, CATEGORY));
        }

        virtual auto ForwardTo(ActorPtr const& to, Message::Category = Message::DEFAULT) const noexcept -> Status = 0;

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename R = typename ATOM::Type::ResultType, typename ... ARGS>
        inline auto Request(ActorPtr const& to, ARGS&& ... args) noexcept -> Future<R> {
            return DoRequest<ATOM, CATEGORY>(to,
                             [](auto&& future) { return Future<R>{future}; },
                             std::forward<ARGS>(args)...);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::DEFAULT, typename R = typename ATOM::Type::ResultType, typename Rep, typename Period, typename ... ARGS>
        inline auto Request(ActorPtr const& to, std::chrono::duration<Rep, Period> timeout, ARGS&& ... args) noexcept -> Future<R> {
            return DoRequest<ATOM, CATEGORY>(to,
                             [&timeout, this](auto&& future) mutable -> Future<R> {
                                return this->StartFutureTimer(InMs(timeout), future);
                             },
                             std::forward<ARGS>(args)...);
        }

        template<typename MSG, typename F, typename R = std::invoke_result_t<F, MSG>>
        inline auto ExpectMsg(F&& f) noexcept -> Future<R> {
            return DoExpectMsg<MSG>(std::forward<F>(f), [](auto&&) { return Status::OK; });
        }

        template<typename MSG, typename F, typename R = std::invoke_result_t<F, MSG>, typename Rep, typename Period>
        inline auto ExpectMsg(std::chrono::duration<Rep, Period> timeout, F&& f) noexcept -> Future<R> {
            return DoExpectMsg<MSG>(std::forward<F>(f), [this, &timeout](auto&& handler) {
                return this->StartExpectMsgTimer(InMs(timeout), handler);
            });
        }

    public:
        template<typename F>
        inline auto After(TimerSpec const& spec, F&& f) noexcept ->  Result<TimerId> {
            return StartTimerWithUserCallback(spec, 1, std::forward<F>(f));
        }

        template<typename F, typename Rep, typename Period>
        inline auto After(std::chrono::duration<Rep, Period> timeout, F&& f) noexcept ->  Result<TimerId> {
            return After(InMs(timeout), std::forward<F>(f));
        }

        template<typename F>
        inline auto Repeat(TimerSpec const& spec, std::size_t repeatTimes, F&& f) noexcept ->  Result<TimerId> {
            return StartTimerWithUserCallback(spec, repeatTimes, std::forward<F>(f));
        }

        template<typename F>
        inline auto Repeat(TimerSpec const& spec, F&& f) noexcept ->  Result<TimerId> {
            return StartTimerWithUserCallback(spec, std::numeric_limits<std::size_t>::max(), std::forward<F>(f));
        }

        template<typename F, typename Rep, typename Period>
        inline auto Repeat(std::chrono::duration<Rep, Period> duration, std::size_t repeatTimes, F&& f) noexcept ->  Result<TimerId> {
            return Repeat(InMs(duration), repeatTimes, std::forward<F>(f));
        }

        template<typename F, typename Rep, typename Period>
        inline auto Repeat(std::chrono::duration<Rep, Period> duration, F&& f) noexcept ->  Result<TimerId> {
            return Repeat(duration, std::numeric_limits<std::size_t>::max(), std::forward<F>(f));
        }

    private:
        template<typename F>
        auto StartTimerWithUserCallback(TimerSpec const& spec, std::size_t repeatTimes, F&& f) noexcept -> Result<TimerId> {
            return StartTimer(spec, repeatTimes,
                              [cb = std::forward<F>(f)](ActorPtr& actor,TimerId const& timerId) mutable -> Status {
                                  return GlobalActorContext::Send<TimeoutMsg>(actor, timerId, std::forward<F>(cb));
                              });
        }

        template<typename ATOM, Message::Category CATEGORY = Message::NORMAL, typename R = typename ATOM::Type::ResultType, typename F, typename ... ARGS>
        auto DoRequest(ActorPtr const& to, F&& f, ARGS&& ... args) noexcept -> Future<R> {
            Promise<R> promise;
            auto status = to.SendMsg(MakeRequest<typename ATOM::MsgType, CATEGORY>(static_cast<ActorPtr const&>(Self()), promise, std::forward<ARGS>(args)...));
            CAF_ASSERT_R(status, (Future<R>{detail::FutureCauseTag{}, status}));
            return f(promise.GetFuture());
        }

        template<typename MSG, typename F, typename R = std::invoke_result_t<F, MSG>, typename CALLBACK>
        auto DoExpectMsg(F&& f, CALLBACK&& callback) noexcept -> Future<R> {
            auto handler = std::make_shared<detail::ExpectMsgHandler<MSG>>();
            CAF_ASSERT_VALID_PTR_R(handler, {});

            auto future = handler->GetFuture();
            RegisterMsgHandler(MSG::ID, handler);
            CAF_ASSERT_R(callback(handler), {});

            return Future<MSG&>{future}.Then(std::forward<F>(f));
        }

        template<typename R>
        auto StartFutureTimer(TimerSpec const& spec, std::shared_ptr<detail::FutureObject<R>> const& future) -> Future<R> {
            Result<TimerId> result = StartTimer(spec, future);
            CAF_ASSERT_TRUE_R(result.Ok(), (Future<R>{detail::FutureCauseTag{}, result.GetStatus()}));
            CAF_ASSERT_R(future->RegisterTimerObserver(*result), (Future<R>{detail::FutureCauseTag{}, status_}));
            return Future<R>{future};
        }

        template<typename MSG>
        auto StartExpectMsgTimer(TimerSpec const& spec, std::shared_ptr<detail::ExpectMsgHandler<MSG>>& handler) -> Status {
            std::shared_ptr<detail::CancellableMsgHandler> cancellableMsgHandler = handler;
            auto result = StartTimer(spec, cancellableMsgHandler);
            CAF_ASSERT_TRUE_R(result.Ok(), result.GetStatus());
            return handler->GetFuture()->RegisterTimerObserver(*result);
        }

    private:
        // DON'T OVERRIDE THOSE METHODS IN YOUR ACTOR!!!
        virtual auto CurrentSender() const noexcept -> ActorPtr = 0;
        virtual auto RegisterMsgHandler(MsgTypeId, std::shared_ptr<detail::CancellableMsgHandler> const&) noexcept -> void = 0;
        virtual auto StartTimer(TimerSpec const& spec, std::size_t repeatTimes, TimeoutCallback&& callback) noexcept -> Result<TimerId> = 0;
        virtual auto StartTimer(TimerSpec const& spec, std::shared_ptr<detail::CancellableMsgHandler> const&) noexcept -> Result<TimerId> = 0;
        virtual auto StartTimer(TimerSpec const& spec, std::shared_ptr<PromiseDoneNotifier> const&) noexcept -> Result<TimerId> = 0;
    };
}

#endif //NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B
