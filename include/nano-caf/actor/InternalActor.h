//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_61F428315F9E41D58DE43222D7AF9BDE
#define NANO_CAF_2_61F428315F9E41D58DE43222D7AF9BDE

#include <nano-caf/actor/ActorHandle.h>

namespace nano_caf::detail {
    template<typename T, typename = void>
    struct ActorHasInit : std::false_type {};

    template<typename T>
    struct ActorHasInit<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().on_init())>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasExit : std::false_type {};

    template<typename T>
    struct ActorHasExit<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().on_exit())>>>
            : std::true_type {};

    template<typename T>
    struct InternalActor : SchedActor, private T {
        template<typename ... ARGS>
        InternalActor(ARGS&&...args)
        : T{std::forward<ARGS>(args)...} {}

        ~SchedActor() = default;

        auto Exit(ExitReason reason) noexcept -> void override {
            SchedActor::Exit_(reason);
        }

        auto Self() const noexcept -> ActorHandle override {
            return this;
        }

        auto Init() noexcept -> void override {
            if constexpr (ActorHasInit<T>::value) {
                T::on_init();
            }
        }

        auto ExitHandler() noexcept -> void override {
            if constexpr (ActorHasExit<T>::value) {
                T::on_exit();
            }
        }

        auto UserDefinedHandleMessage(Message& msg) noexcept -> void override {
            T::handle_message(msg);
        }
    };
}

#endif //NANO_CAF_2_61F428315F9E41D58DE43222D7AF9BDE
