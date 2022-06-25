//
// Created by Darwin Yuan on 2022/6/25.
//

#ifndef NANO_CAF_2_180785809842472AB4533845B39E05D2
#define NANO_CAF_2_180785809842472AB4533845B39E05D2

#include <cstdint>


namespace nano_caf {
    struct TimerId {
        explicit TimerId(uint64_t id) : id_{id} {}

        auto operator==(TimerId& rhs) const { return id_ == rhs.id_; }
        auto operator!=(TimerId& rhs) const { return id_ != rhs.id_; }
        auto operator<(TimerId& rhs) const  { return id_ < rhs.id_; }

    private:
        uint64_t id_;
    };
}



#endif //NANO_CAF_2_180785809842472AB4533845B39E05D2
