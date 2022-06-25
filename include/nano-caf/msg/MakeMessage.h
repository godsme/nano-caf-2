//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_5D1CAD1939944F949D71884E5131A588
#define NANO_CAF_2_5D1CAD1939944F949D71884E5131A588

#include <nano-caf/msg/Message.h>

namespace nano_caf::detail {

    template<typename T, Message::Category CATEGORY>
    struct MessageBase : Message {
        MessageBase() : Message(T::ID, CATEGORY) {}
        MessageBase(ActorPtr const& sender)
        : Message(sender, T::ID, CATEGORY) {}
    };


    template<typename T, Message::Category CATEGORY, typename = void>
    struct MessageEntity : MessageBase<T, CATEGORY> {
        template<typename ... ARGS>
        MessageEntity(ActorPtr const& sender, ARGS&&...args)
            : MessageBase<T, CATEGORY>{sender}
            , value{std::forward<ARGS>(args)...}
        {}

        template<typename ... ARGS>
        MessageEntity(ARGS&&...args)
            : value{std::forward<ARGS>(args)...}
        {}

    private:
        auto GetBody() const noexcept -> const void* override {
            return reinterpret_cast<const void*>(&value);
        }

    private:
        T value;
    };

    template<typename T, Message::Category CATEGORY>
    struct MessageEntity<T, CATEGORY, std::enable_if_t<std::is_class_v<T>>>
            : MessageBase<T, CATEGORY>, private T {

        template<typename ... ARGS>
        MessageEntity(ActorPtr const& sender, ARGS&&...args)
            : MessageBase<T, CATEGORY>{sender}
            , T{std::forward<ARGS>(args)...}
        {}

        template<typename ... ARGS>
        MessageEntity(ARGS&&...args)
            : T{std::forward<ARGS>(args)...}
        {}

    private:
        auto GetBody() const noexcept -> const void* override {
            return reinterpret_cast<const void*>(static_cast<const T*>(this));
        }
    };

    template<typename T, typename HANDLER, Message::Category CATEGORY>
    struct RequestEntity : MessageEntity<T, CATEGORY> {
        using Parent = MessageEntity<T, CATEGORY>;
        using Handler = std::decay_t<HANDLER>;

        template<typename ... ARGS>
        RequestEntity(ActorPtr const& sender, HANDLER&& handler, ARGS&&...args)
                : Parent{sender, std::forward<ARGS>(args)...}
                , m_handler{std::forward<HANDLER>(handler)} {}

        template<typename ... ARGS>
        RequestEntity(HANDLER&& handler, ARGS&&...args)
                : Parent{std::forward<ARGS>(args)...}
                , m_handler{std::move(handler)}
        {}

        auto GetPromise() const noexcept -> void* override {
            return reinterpret_cast<void*>(const_cast<Handler*>(&m_handler));
        }

        virtual auto OnDiscard() noexcept -> void override {
            auto&& notifier = reinterpret_cast<FailNotifier&>(m_handler);
            notifier.OnFail(Status::DISCARDED, Parent::sender);
        }

    private:
        static_assert(std::is_base_of_v<AbstractPromise<typename T::ResultType>, Handler>);
        Handler m_handler;
    };
}

namespace nano_caf {
    template<typename T, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
    auto MakeMessage(ARGS&& ... args) -> Message* {
        return new detail::MessageEntity<T, CATEGORY>(std::forward<ARGS>(args)...);
    }

    template<typename T, Message::Category CATEGORY = Message::NORMAL, typename ... ARGS>
    auto MakeMessage(ActorPtr const& sender, ARGS&& ... args) -> Message* {
        return new detail::MessageEntity<T, CATEGORY>(sender, std::forward<ARGS>(args)...);
    }

    template<typename T, Message::Category CATEGORY = Message::NORMAL, typename HANDLER, typename ... ARGS>
    auto MakeRequest(ActorPtr const& sender, HANDLER&& handler, ARGS&& ... args) -> Message* {
        return new detail::RequestEntity<T, HANDLER, CATEGORY>(sender, std::forward<HANDLER>(handler), std::forward<ARGS>(args)...);
    }

    template<typename T, Message::Category CATEGORY = Message::NORMAL, typename HANDLER, typename ... ARGS>
    auto MakeRequest(HANDLER&& handler, ARGS&& ... args) -> Message* {
        return new detail::RequestEntity<T, HANDLER, CATEGORY>(std::forward<HANDLER>(handler), std::forward<ARGS>(args)...);
    }
}

#endif //NANO_CAF_2_5D1CAD1939944F949D71884E5131A588
