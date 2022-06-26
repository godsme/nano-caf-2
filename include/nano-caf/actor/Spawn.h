//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
#define NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/actor/detail/ExpectOnceMsgHandlers.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/actor/Behavior.h>

namespace nano_caf::detail {
    template<typename T, typename = void>
    struct ActorHasInit : std::false_type {};

    template<typename T>
    struct ActorHasInit<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().OnInit())>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasExit : std::false_type {};

    template<typename T>
    struct ActorHasExit<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().OnExit(ExitReason::NORMAL))>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasHandleMsg : std::false_type {};

    template<typename T>
    struct ActorHasHandleMsg<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().HandleMessage(std::declval<Message&>()))>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasGetBehavior : std::false_type {};

    template<typename T>
    struct ActorHasGetBehavior<T, std::enable_if_t<std::is_same_v<Behavior, decltype(std::declval<T>().GetBehavior())>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasInitBehavior : std::false_type {};

    template<typename T>
    struct ActorHasInitBehavior<T, std::enable_if_t<std::is_same_v<Behavior, std::decay_t<decltype(T::INIT_Behavior)>>>>
            : std::true_type {};

    template<typename T>
    struct InternalActor : SchedActor, T {
    private:
        static_assert(!(ActorHasInitBehavior<T>::value && ActorHasGetBehavior<T>::value),
                "you can only either defined GetBehavior() or INIT_Behavior in a certain actor");
        using SchedActor::m_currentMsg;
    public:
        template<typename ... ARGS>
        InternalActor(bool sync, ARGS&&...args)
            : SchedActor{sync}
            , T{std::forward<ARGS>(args)...}
        {
            if constexpr(!ActorHasInit<T>::value) {
                SchedActor::m_inited = true;
            }

            if constexpr(ActorHasGetBehavior<T>::value) {
                m_behavior = T::GetBehavior();
            } else if constexpr(ActorHasInitBehavior<T>::value) {
                m_behavior = T::INIT_Behavior;
            }
        }

        auto Exit(ExitReason reason) noexcept -> void override {
            SchedActor::Exit_(reason);
        }

        auto Self() const noexcept -> ActorHandle override {
            return ActorHandle{const_cast<InternalActor*>(this), true};
        }

        auto InitHandler() noexcept -> void override {
            if constexpr (ActorHasInit<T>::value) {
                T::OnInit();
            }
        }

        auto ExitHandler(ExitReason reason) noexcept -> void override {
            if constexpr (ActorHasExit<T>::value) {
                T::OnExit(reason);
            }
        }

        auto CurrentSender() const noexcept -> ActorHandle override {
            if(m_currentMsg == nullptr) return {};
            auto sender = m_currentMsg->sender.Lock();
            return sender ? ActorHandle{sender.Get()} : ActorHandle{};
        }

        auto RegisterExpectOnceHandler(MsgTypeId msgId, detail::MsgHandler* handler) noexcept -> void override {
            m_expectOnceMsgHandlers.AddHandler(msgId, handler);
        }

        auto HandleUserDefinedMsg(Message& msg) noexcept -> void {
            if(m_expectOnceMsgHandlers.HandleMsg(msg)) return;
            if constexpr(ActorHasGetBehavior<T>::value || ActorHasInitBehavior<T>::value) {
                if(m_behavior.HandleMsg(msg)) return;
            }

            msg.OnDiscard();
        }

        auto UserDefinedHandleMessage(Message& msg) noexcept -> void override {
            switch(msg.id) {
                case BootstrapMsg::ID: break; // ignore
                case FutureDoneNotify::ID: {
                    auto notifier = msg.Body<FutureDoneNotify>()->notifier;
                    notifier->Commit();
                    break;
                }
                default: {
                    HandleUserDefinedMsg(msg);
                    break;
                }
            }
        }

        auto StartTimer(TimerSpec const& spec, bool periodic, TimeoutCallback&& callback) -> nano_caf::Result<TimerId> override {
            auto result = ActorSystem::Instance().StartTimer(Self(), spec, periodic, std::move(callback));
            if(result.Ok()) {
                timerUsed = true;
            }
            return result;
        }

        auto StopTimer(TimerId timerId) noexcept -> void override {
            ActorSystem::Instance().StopTimer((intptr_t)this, timerId);
        }

        auto ChangeBehavior(Behavior const& to) noexcept -> void override {
            m_behavior = to;
        }

        ~InternalActor() {
            if(timerUsed) {
                ActorSystem::Instance().ClearActorTimer((intptr_t)this);
            }
        }

    private:
        Behavior m_behavior;
        ExpectOnceMsgHandlers m_expectOnceMsgHandlers;
        bool timerUsed{false};
    };
}

namespace nano_caf {
    template<typename T, bool SYNC = false, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto Spawn(ARGS&& ... args) -> ActorHandle {
        using ActorObject = detail::InternalActor<T>;
        auto ptr = MakeShared<ActorObject, MEM_ALLOCATOR>(SYNC, std::forward<ARGS>(args)...);
        return ActorHandle{ptr ? ptr.Get() : nullptr};
    }
}

#endif //NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
