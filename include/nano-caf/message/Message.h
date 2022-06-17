//
// Created by Darwin Yuan on 2022/6/16.
//

#ifndef NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
#define NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177

#include "nano-caf/util/ListElem.h"

using MessageId = uint16_t;

namespace nano_caf {
    struct Message : ListElem<Message>  {
        enum class Category {
            NORMAL,
            URGENT
        };

        template<typename BODY>
        auto Body() const noexcept -> BODY const* {
            return nullptr;
        }

        MessageId m_id;
        Category m_category;
    };
}

#endif //NANO_CAF_2_D6D87D954DAC419D8DBBF6CD13EC7177
