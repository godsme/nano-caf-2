//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B
#define NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B

#include <nano-caf/msg/Message.h>
#include <nano-caf/actor/ActorHandle.h>

namespace nano_caf {
    struct Actor {
        virtual ~Actor() = default;

    protected:
        template<typename T, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
        inline auto Send(ActorHandle const& to, ARGS&& ... args) const noexcept -> Status {
            return to.Send<T, CATEGORY>(Self(), std::forward<ARGS>(args)...);
        }

        template<typename T, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
        inline auto Reply(ARGS&& ... args) const noexcept -> Status {
            return Send<T, CATEGORY>(CurrentSender(), std::forward<ARGS>(args)...);
        }

    protected:
        virtual auto Self() const noexcept -> ActorHandle = 0;
        virtual auto Exit(ExitReason) noexcept -> void = 0;
        virtual auto HandleMessage(Message&) noexcept -> void = 0;
    private:
        virtual auto CurrentSender() const noexcept -> ActorHandle = 0;
    };
}

#endif //NANO_CAF_2_404238781FDD4EA28C56C2A4FC1E624B
