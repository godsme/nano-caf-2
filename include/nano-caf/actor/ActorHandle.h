//
// Created by Darwin Yuan on 2022/6/20.
//

#ifndef NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
#define NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2

#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/msg/MakeMessage.h>
#include <nano-caf/async/Requester.h>
#include <nano-caf/msg/Atom.h>
#include <nano-caf/Status.h>
#include <chrono>

namespace nano_caf {
    struct ActorHandle : private ActorPtr {
        using ActorPtr::ActorPtr;

        using ActorPtr::operator bool;

        ActorHandle(ActorPtr&& rhs) : ActorPtr(std::move(rhs)) {}
        ActorHandle(ActorHandle const&& rhs) : ActorPtr(rhs) {}

        ActorHandle(ActorHandle&& rhs) : ActorPtr(std::move(rhs)) {}
        ActorHandle(ActorHandle const& rhs) : ActorPtr(rhs) {}

        auto operator=(ActorHandle&& another) noexcept -> ActorHandle& {
            ActorPtr::operator=(std::move(another));
            return *this;
        }

        auto operator=(ActorHandle const& another) noexcept -> ActorHandle& {
            ActorPtr::operator=(another);
            return *this;
        }

        auto ActorId() const noexcept -> intptr_t {
            return reinterpret_cast<intptr_t>(ActorPtr::m_ptr);
        }

        auto ToWeakPtr() const noexcept -> ActorWeakPtr {
            return ActorWeakPtr{*this};
        }

        template<typename MSG, Message::Category CATEGORY = Message::NORMAL, std::enable_if_t<!std::is_base_of_v<nano_caf::AtomSignature, MSG>, bool> = true, typename ... ARGS>
        auto Send(ARGS&& ... args) const noexcept -> Status {
            return SendMsg(MakeMessage<MSG, CATEGORY>(std::forward<ARGS>(args)...));
        }

        template<typename ATOM, Message::Category CATEGORY = Message::NORMAL, std::enable_if_t<std::is_base_of_v<nano_caf::AtomSignature, ATOM>, bool> = true, typename ... ARGS>
        auto Send(ARGS&& ... args) const noexcept -> Status {
            return SendMsg(MakeMessage<typename ATOM::MsgType, CATEGORY>(std::forward<ARGS>(args)...));
        }

        template<typename MSG, Message::Category CATEGORY = Message::NORMAL, std::enable_if_t<!std::is_base_of_v<nano_caf::AtomSignature, MSG>, bool> = true, typename ... ARGS>
        auto Send(ActorHandle const& sender, ARGS&& ... args) const noexcept -> Status {
            return SendMsg(MakeMessage<MSG, CATEGORY>(static_cast<ActorPtr const&>(sender), std::forward<ARGS>(args)...));
        }

        template<typename ATOM, Message::Category CATEGORY = Message::NORMAL, std::enable_if_t<std::is_base_of_v<nano_caf::AtomSignature, ATOM>, bool> = true, typename ... ARGS>
        auto Send(ActorHandle const& sender, ARGS&& ... args) const noexcept -> Status {
            return SendMsg(MakeMessage<typename ATOM::MsgType, CATEGORY>(static_cast<ActorPtr const&>(sender), std::forward<ARGS>(args)...));
        }

        template<typename ATOM, Message::Category CATEGORY = Message::NORMAL, typename R = typename ATOM::Type::ResultType, typename ... ARGS>
        auto Request(ARGS&& ... args) -> Result<R> {
            return DoRequest<ATOM, R, CATEGORY>([](auto&& future) -> Result<R> {
                return future.get();
            }, std::forward<ARGS>(args)...);
        }

        template<typename ATOM, Message::Category CATEGORY = Message::NORMAL, typename R = typename ATOM::Type::ResultType, typename Rep, typename Period, typename ... ARGS>
        auto Request(std::chrono::duration<Rep, Period> duration, ARGS&& ... args) -> Result<R> {
            return DoRequest<ATOM, R, CATEGORY>([&duration](auto&& future) {
                if(future.wait_for(duration) != std::future_status::ready) {
                    return Result<R>{ResultTag::CAUSE, Status::TIMEOUT};
                }
                return future.get();
            }, std::forward<ARGS>(args)...);
        }

        auto Wait(ExitReason& reason) noexcept -> Status;

        auto Exit(ExitReason reason) noexcept -> Status;

        using ActorPtr::Release;

        auto SendMsg(Message*) const noexcept -> Status;

    private:
        friend struct Actor;

        template<typename MSG, Message::Category CATEGORY = Message::NORMAL, typename HANDLER, typename ... ARGS>
        auto DoRequest(ActorHandle const& sender, HANDLER&& handler, ARGS&& ... args) const noexcept -> Status {
            return SendMsg(MakeRequest<MSG, CATEGORY>(static_cast<ActorPtr const&>(sender), std::forward<HANDLER>(handler), std::forward<ARGS>(args)...));
        }

        template<typename ATOM, typename R, Message::Category CATEGORY = Message::NORMAL, typename F, typename ... ARGS>
        auto DoRequest(F&& f, ARGS&& ... args) -> Result<R> {
            Requester<R> requester{};
            auto&& future = requester.GetFuture();
            auto status = SendMsg(MakeRequest<typename ATOM::MsgType, CATEGORY>(std::move(requester), std::forward<ARGS>(args)...));
            if(status != Status::OK) {
                return Result<R>{ResultTag::CAUSE, status};
            }

            return f(future);
        }
    };
}

#endif //NANO_CAF_2_18EE9B4948A14E4F89BFE3893421ECE2
