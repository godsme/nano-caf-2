//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
#define NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/actor/Behavior.h>
#include <nano-caf/actor/BlockingActor.h>
#include <nano-caf/actor/NonblockingActor.h>
#include <nano-caf/actor/detail/ActorTimerContext.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>

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
    struct ActorHasGetBehavior : std::false_type {};

    template<typename T>
    struct ActorHasGetBehavior<T, std::enable_if_t<std::is_same_v<Behavior, decltype(std::declval<T>().GetBehavior())>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasInitBehavior : std::false_type {};

    template<typename T>
    struct ActorHasInitBehavior<T, std::enable_if_t<std::is_same_v<Behavior, std::decay_t<decltype(T::INIT_Behavior)>>>>
            : std::true_type {};

    template<typename T, typename ACTOR>
    struct ActorObject final : ACTOR, T, private detail::ActorTimerContext {
    private:
        static_assert(!(ActorHasInitBehavior<T>::value && ActorHasGetBehavior<T>::value),
                "you can only either defined GetBehavior() or INIT_Behavior in a certain actor");
        using ACTOR::m_currentMsg;

        auto Self_() const noexcept -> ActorHandle {
            return ActorHandle{const_cast<ActorObject*>(this), true};
        }

    public:
        template<typename ... ARGS>
        ActorObject(bool sync, ARGS&&...args)
            : ACTOR{sync}
            , T{std::forward<ARGS>(args)...}
        {
            if constexpr(!ActorHasInit<T>::value) {
                ACTOR::m_inited = true;
            }

            if constexpr(ActorHasGetBehavior<T>::value) {
                m_behavior = T::GetBehavior();
            } else if constexpr(ActorHasInitBehavior<T>::value) {
                m_behavior = T::INIT_Behavior;
            }
        }

        auto Self() const noexcept -> ActorHandle override {
            return Self_();
        }

        auto Exit(ExitReason reason) noexcept -> void override {
            SchedActor::Exit_(reason);
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
            return m_currentMsg->sender.Lock().Get();
        }

        auto RegisterMsgHandler(MsgTypeId msgId, std::shared_ptr<detail::CancellableMsgHandler> const& handler) noexcept -> void override {
            detail::ActorTimerContext::AddHandler(msgId, handler);
        }

        auto HandleUserDefinedMsg(Message& msg) noexcept -> bool override {
            if(detail::ActorTimerContext::HandleMsg(msg)) {
                return true;
            }
            if constexpr(ActorHasGetBehavior<T>::value || ActorHasInitBehavior<T>::value) {
                if(m_behavior.HandleMsg(msg)) return true;
            }
            return false;
        }

        auto StartTimer(TimerSpec const& spec, std::size_t repeatTimes, TimeoutCallback&& callback) noexcept -> nano_caf::Result<TimerId> override {
            return detail::ActorTimerContext::StartTimer(Self_(), spec, repeatTimes, std::move(callback));
        }

        auto StartTimer(TimerSpec const& spec, std::shared_ptr<detail::CancellableMsgHandler> const& handler) noexcept -> Result<TimerId> override {
            return detail::ActorTimerContext::StartExpectMsgTimer(Self_(), spec, handler);
        }

        auto StartTimer(TimerSpec const& spec, std::shared_ptr<PromiseDoneNotifier> const& notifier) noexcept -> Result<TimerId> override {
            return detail::ActorTimerContext::StartFutureTimer(Self_(), spec, notifier);
        }

        auto ChangeBehavior(Behavior const& to) noexcept -> void override {
            m_behavior = to;
        }

        auto ForwardTo(ActorHandle const& to, Message::Category category = Message::DEFAULT) const noexcept -> Status override {
            CAF_ASSERT_VALID_PTR(m_currentMsg);
            if(category != Message::DEFAULT) {
                m_currentMsg->category = category;
            }
            return to.SendMsg(m_currentMsg.release());
        }

        ~ActorObject() {
            detail::ActorTimerContext::ClearAllTimers((intptr_t)this);
        }

    private:
        Behavior m_behavior;
    };
}

namespace nano_caf {
    namespace detail {
        template<typename T, typename ActorType, bool SYNC, typename MEM_ALLOCATOR, typename ... ARGS>
        auto DoSpawn(ARGS&& ... args) -> ActorHandle {
            using ActorObject = detail::ActorObject<T, ActorType>;
            auto ptr = MakeShared<ActorObject, detail::ActorCtlBlock, MEM_ALLOCATOR>(SYNC, std::forward<ARGS>(args)...);
            CAF_ASSERT_TRUE_NIL(ptr);
            return ptr.Get();
        }
    }

    template<typename T, bool SYNC = false, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto Spawn(ARGS&& ... args) -> ActorHandle {
        return detail::DoSpawn<T, NonblockingActor, SYNC, MEM_ALLOCATOR>(std::forward<ARGS>(args)...);
    }

    template<typename T, bool SYNC = true, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto SpawnBlockingActor(ARGS&& ... args) -> ActorHandle {
        return detail::DoSpawn<T, BlockingActor, SYNC, MEM_ALLOCATOR>(std::forward<ARGS>(args)...);
    }
}

#endif //NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
