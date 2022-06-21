//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
#define NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4

#include <nano-caf/actor/ActorHandle.h>

namespace nano_caf::detail {
    template<typename T, typename = void>
    struct ActorHasInit : std::false_type {};

    template<typename T>
    struct ActorHasInit<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().OnInit())>>>
            : std::true_type {};

    template<typename T, typename = void>
    struct ActorHasExit : std::false_type {};

    template<typename T>
    struct ActorHasExit<T, std::enable_if_t<std::is_void_v<decltype(std::declval<T>().OnExit(ExitReason::NORMAL))>>>
            : std::true_type {};

    template<typename T>
    struct InternalActor : SchedActor, private T {
        template<typename ... ARGS>
        InternalActor(ARGS&&...args)
            : T{std::forward<ARGS>(args)...}
        {}

        virtual ~InternalActor() = default;

        auto Exit(ExitReason reason) noexcept -> void override {
            SchedActor::Exit_(reason);
        }

        auto Self() const noexcept -> ActorHandle override {
            return this;
        }

        auto Init() noexcept -> void override {
            if constexpr (ActorHasInit<T>::value) {
                T::OnInit();
            }
        }

        auto ExitHandler(ExitReason reason) noexcept -> void override {
            if constexpr (ActorHasExit<T>::value) {
                T::OnExit(reason);
            }
        }

        auto UserDefinedHandleMessage(Message& msg) noexcept -> void override {
            T::HandleMessage(msg);
        }
    };
}

namespace nano_caf {
    template<typename T, typename MEM_ALLOCATOR = DefaultMemAllocator, typename ... ARGS>
    auto Spawn(ARGS&& ... args) -> ActorHandle {
        using ActorObject = detail::InternalActor<T>;
        auto&& handle = MakeShared<ActorObject, MEM_ALLOCATOR>(std::forward<ARGS>(args)...);
        if(!handle) {
            return {};
        } else {
            return {handle.Get()};
        }
    }
}

#endif //NANO_CAF_2_EE44DD5E09F64C729C20F495B1ECD2D4
