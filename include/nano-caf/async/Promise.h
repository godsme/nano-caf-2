//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_C601717AA6D74F32826C395175572D97
#define NANO_CAF_2_C601717AA6D74F32826C395175572D97

#include <nano-caf/async/AbstractPromise.h>
#include <nano-caf/async/detail/FutureObject.h>

namespace nano_caf {
    template<typename T>
    struct Promise : AbstractPromise<T> {
        auto GetFuture() noexcept -> Future<T> {
            if(!m_future) {
                m_future = std::make_shared<detail::FutureObject<T>>();
            }
            return {m_future};
        }

    private:
        std::shared_ptr<detail::FutureObject<T>> m_future;
    };
}

#endif //NANO_CAF_2_C601717AA6D74F32826C395175572D97
