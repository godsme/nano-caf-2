//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_6403FD8A5D7543428FFC9DBCA5B66985
#define NANO_CAF_2_6403FD8A5D7543428FFC9DBCA5B66985

namespace nano_caf {
    struct PromiseDoneNotifier {
        virtual auto Commit() noexcept -> void = 0;
        virtual ~PromiseDoneNotifier() = default;
    };
}

#endif //NANO_CAF_2_6403FD8A5D7543428FFC9DBCA5B66985
