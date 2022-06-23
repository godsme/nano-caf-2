//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
#define NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177

#include <nano-caf/util/ListElem.h>
#include <nano-caf/actor/ActorPtr.h>
#include <nano-caf/msg/MsgTypeId.h>
#include <nano-caf/msg/RequestConcept.h>

namespace nano_caf {
    template<typename T>
    struct AbstractPromise;

    struct Message : ListElem<Message>  {
        enum Category : uint64_t {
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

        template<typename BODY, std::enable_if_t<IS_REQUEST<BODY>>>
        auto Promise() const noexcept -> AbstractPromise<typename BODY::ResultType>* {
            return nullptr;
        }

        virtual ~Message() = default;

    private:
        virtual auto GetBody() const noexcept -> void const* = 0;

    public:
        ActorWeakPtr sender;
        MsgTypeId id;
        Category category;
    };
}

#endif //NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
