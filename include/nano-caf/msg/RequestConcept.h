//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_0449DE68BD0B4368AF104B7C48B1FA69
#define NANO_CAF_2_0449DE68BD0B4368AF104B7C48B1FA69

#include <type_traits>

namespace nano_caf {
    template<typename T, typename = void>
    constexpr bool IS_REQUEST = false;

    template<typename T>
    constexpr bool IS_REQUEST<T, std::enable_if_t<T::Is_Request>> = true;
}

#endif //NANO_CAF_2_0449DE68BD0B4368AF104B7C48B1FA69
