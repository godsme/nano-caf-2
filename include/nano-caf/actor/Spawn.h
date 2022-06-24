//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
#define NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4

#include <nano-caf/actor/ActorHandle.h>
#include <nano-caf/actor/detail/ExpectOnceMsgHandlers.h>
#include <nano-caf/msg/PredefinedMsgs.h>

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

    template<typename T>
    struct InternalActor : SchedActor, T {
    private:
        static_assert(ActorHasHandleMsg<T>::value, "missing `auto HandleMessage(Message&) noexcept -> void`");
    private:
        using SchedActor::m_currentMsg;
    public:
        template<typename ... ARGS>
        InternalActor(bool sync, ARGS&&...args)
            : SchedActor{sync}
            , T{std::forward<ARGS>(args)...}
        {}

        virtual ~InternalActor() = default;

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

        auto UserDefinedHandleMessage(Message& msg) noexcept -> void override {
            switch(msg.id) {
                case FutureDoneNotify::ID: {
                    auto notifier = msg.Body<FutureDoneNotify>()->notifier;
                    notifier->Commit();
                    break;
                }
                default: {
                    if(!m_expectOnceMsgHandlers.HandleMsg(msg)) {
                        T::HandleMessage(msg);
                    }
                    break;
                }
            }
        }

    private:
        ExpectOnceMsgHandlers m_expectOnceMsgHandlers;
    };
}

namespace nano_caf {
    template<typename T, bool SYNC = false, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto Spawn(ARGS&& ... args) -> ActorHandle {
        using ActorObject = detail::InternalActor<T>;
        auto&& handle = MakeShared<ActorObject, MEM_ALLOCATOR>(SYNC, std::forward<ARGS>(args)...);
        if(!handle) {
            return {};
        } else {
            if constexpr(detail::ActorHasInit<T>::value) {
                if(handle->Init() != TaskResult::SUSPENDED) {
                    return {};
                }
            }
            return ActorHandle{handle.Get()};
        }
    }
}

#endif //NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
