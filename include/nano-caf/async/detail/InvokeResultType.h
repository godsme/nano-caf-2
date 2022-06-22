//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_AE68443BC30B416596FDC16FAF3DFE93
#define NANO_CAF_2_AE68443BC30B416596FDC16FAF3DFE93

#include <type_traits>

namespace nano_caf::detail {
    template <typename F, typename A>
    struct InvokeResult {
        using Type = std::invoke_result_t<F, A>;
    };

    template <typename F>
    struct InvokeResult<F, void> {
        using Type = std::invoke_result_t<F>;
    };

    template <typename F, typename A>
    using InvokeResultType = typename InvokeResult<F, A>::Type;
}

#endif //NANO_CAF_2_AE68443BC30B416596FDC16FAF3DFE93
