//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
#define NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4

#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/actor/Behavior.h>
#include <nano-caf/actor/BlockingActor.h>
#include <nano-caf/actor/NonblockingActor.h>
#include <nano-caf/actor/detail/ActorTimerContext.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>
#include <nano-caf/util/MemberDetector.h>

namespace nano_caf::detail {
    /////////////////////////////////////////////////////////////
    DEF_METHOD_DETECTOR(Actor_Has_Init, void, OnActorInit);
    DEF_METHOD_DETECTOR(Actor_Has_Exit, void, OnActorInit, ExitReason);
    DEF_METHOD_DETECTOR(Actor_Has_OnExit, void, OnActorExit);
    DEF_METHOD_DETECTOR(Actor_Has_GetBehavior, Behavior, GetBehavior);
    DEF_MEMBER_VAR_DETECTOR(Actor_Has_InitBehavior, Behavior, INIT_Behavior);

    /////////////////////////////////////////////////////////////
    template<typename T, typename ACTOR>
    struct ActorObject final : ACTOR, T, private detail::ActorTimerContext {
    private:
        using ACTOR::m_currentMsg;

        auto Self_() const noexcept -> ActorPtr {
            return ActorPtr{const_cast<ActorObject*>(this), true};
        }

    public:
        template<typename ... ARGS>
        ActorObject(bool sync, ARGS&&...args)
            : ACTOR{sync}
            , T{std::forward<ARGS>(args)...}
        {
            if constexpr(!Actor_Has_Init<T>) {
                ACTOR::m_inited = true;
            }

            if constexpr(Actor_Has_GetBehavior<T>) {
                m_behavior = T::GetBehavior();
            } else if constexpr(Actor_Has_InitBehavior<T>) {
                m_behavior = T::INIT_Behavior;
            }
        }

        auto Self() const noexcept -> ActorPtr override {
            return Self_();
        }

        auto Exit(ExitReason reason) noexcept -> void override {
            SchedActor::Exit_(reason);
        }

        auto InitHandler() noexcept -> void override {
            if constexpr (Actor_Has_Init<T>) {
                T::OnActorInit();
            }
        }

        auto ExitHandler(ExitReason reason) noexcept -> void override {
            if constexpr (Actor_Has_Exit<T>) {
                T::OnActorExit(reason);
            } else if constexpr(Actor_Has_OnExit<T>) {
                T::OnActorExit();
            }
        }

        auto CurrentSender() const noexcept -> ActorPtr override {
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

            if constexpr(Actor_Has_GetBehavior<T> || Actor_Has_InitBehavior<T>) {
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

        auto ForwardTo(ActorPtr const& to, Message::Category category = Message::DEFAULT) const noexcept -> Status override {
            CAF_ASSERT_VALID_PTR(m_currentMsg);
            if(category != Message::DEFAULT) {
                m_currentMsg->category = category;
            }
            return to.SendMsg(m_currentMsg.release());
        }

        ~ActorObject() {
            ACTOR::Close(ExitReason::ABNORMAL);
            detail::ActorTimerContext::ClearAllTimers((intptr_t)this);
        }

    private:
        Behavior m_behavior;
    };
}

namespace nano_caf {
    namespace detail {
        template<typename T, typename ActorType, bool SYNC, typename MEM_ALLOCATOR, typename ... ARGS>
        auto DoSpawn(ARGS&& ... args) -> ActorPtr {
            using ActorObject = detail::ActorObject<T, ActorType>;
            auto p = MakeShared<ActorObject, detail::ActorCtlBlock, MEM_ALLOCATOR>(SYNC, std::forward<ARGS>(args)...);
            return {p.Get()};
        }
    }

    template<typename T, bool SYNC = false, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto Spawn(ARGS&& ... args) -> ActorPtr {
        return detail::DoSpawn<T, NonblockingActor, SYNC, MEM_ALLOCATOR>(std::forward<ARGS>(args)...);
    }

    template<typename T, bool SYNC = true, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto SpawnBlockingActor(ARGS&& ... args) -> ActorPtr {
        return detail::DoSpawn<T, BlockingActor, SYNC, MEM_ALLOCATOR>(std::forward<ARGS>(args)...);
    }
}

#endif //NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
