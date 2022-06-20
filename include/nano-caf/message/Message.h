//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
#define NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177

#include <nano-caf/util/ListElem.h>
#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/util/WeakPtr.h>
#include <nano-caf/message/MsgTypeId.h>

namespace nano_caf {
    struct Message : ListElem<Message>  {
        enum Category : uint64_t {
            NORMAL,
            URGENT
        };

        Message(MsgTypeId typeId,  Category category = Category::NORMAL)
            : m_id(typeId)
            , m_category(category)
        {}

        Message(SharedPtr<SchedActor> const& sender, MsgTypeId typeId,  Category category = Category::NORMAL)
                : m_id(typeId)
                , m_category(category)
        {}

        template<typename BODY>
        auto Body() const noexcept -> BODY const* {
            return m_id == BODY::TypeId ? reinterpret_cast<BODY const*>(GetBody()) : nullptr;
        }

        virtual ~Message() = default;

    private:
        virtual auto GetBody() const noexcept -> void const* = 0;

    public:
        WeakPtr<SchedActor> m_sender;
        MsgTypeId m_id;
        Category m_category;
    };
}

#endif //NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
