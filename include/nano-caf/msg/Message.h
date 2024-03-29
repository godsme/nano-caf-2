//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
#define NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177

#include <nano-caf/util/ListElem.h>
#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/msg/MsgTypeId.h>
#include <nano-caf/msg/RequestConcept.h>
#include <nano-caf/async/AbstractPromise.h>

namespace nano_caf {
    struct Message : ListElem<Message>  {
        enum Category : uint64_t {
            DEFAULT,
            NORMAL,
            URGENT
        };

        explicit Message(MsgTypeId id,  Category category = Category::NORMAL)
            : id(id)
            , category(category)
        {}

        Message(ActorPtr const& sender, MsgTypeId id,  Category category = Category::NORMAL)
                : sender{sender}
                , id{id}
                , category{category}
        {}

        template<typename BODY>
        auto Body() const noexcept -> BODY const* {
            return id == BODY::ID ? reinterpret_cast<BODY const*>(GetBody()) : nullptr;
        }

        template<typename BODY>
        auto Body() noexcept -> BODY* {
            return const_cast<BODY*>(const_cast<Message const*>(this)->template Body<BODY>());
        }

        template<typename BODY, typename = std::enable_if_t<IS_REQUEST<BODY>>>
        auto Promise() const noexcept -> AbstractPromise<typename BODY::ResultType>* {
            return id == BODY::ID ? reinterpret_cast<AbstractPromise<typename BODY::ResultType>*>(GetPromise()) : nullptr;
        }

        virtual auto OnDiscard() noexcept -> void {}

        virtual ~Message() = default;

    private:
        virtual auto GetBody() const noexcept -> void const* = 0;
        virtual auto GetPromise() const noexcept -> void* { return nullptr; }

    public:
        ActorWeakPtr sender;
        MsgTypeId id;
        Category category;
    };
}

#endif //NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
