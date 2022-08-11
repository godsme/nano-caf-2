//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_142192B507274CADBFFCBEC979A70E95
#define NANO_CAF_2_142192B507274CADBFFCBEC979A70E95

#include <nano-caf/msg/Atom.h>
#include <nano-caf/util/CallableTrait.h>
#include <nano-caf/actor/detail/MsgTypeTrait.h>
#include <nano-caf/actor/detail/BehaviorBase.h>
#include <nano-caf/actor/detail/MsgHandler.h>
#include <nano-caf/async/Future.h>

namespace nano_caf::detail {
    template<typename F, typename = void>
    struct BehaviorTrait;

    template<typename T>
    constexpr bool NonConstLRef = !std::is_const_v<T> && std::is_lvalue_reference_v<T>;

    template<typename F, typename = void>
    constexpr bool IS_ATOM_PATTERN = false;

    template<typename F>
    constexpr bool IS_ATOM_PATTERN<F, std::enable_if_t<IS_MSG_ATOM<std::decay_t<FirstArg<F>>>>> = true;

    template<typename F>
    struct BehaviorTrait<F, std::enable_if_t<IS_ATOM_PATTERN<F>>> {
    private:
        static_assert(!NonConstLRef<FirstArg<F>>, "the atom type cannot be non-const-lvalue-ref");
        using AtomType = std::decay_t<FirstArg<F>>;
        using MsgType = typename AtomType::MsgType;
        using FieldsTypes = typename MsgTypeTrait<MsgType>::FieldsTypes;

        template <typename ... Ts>
        using Invokable = std::is_invocable<F, AtomType, std::decay_t<Ts>...>;

        static_assert(FieldsTypes::template ExportTo<Invokable>::value, "parameters & message don't match");

        template <typename ... Ts>
        using InvokeResult_f = std::invoke_result_t<F, AtomType, Ts...>;
        using InvokeResult = typename FieldsTypes::template ExportTo<InvokeResult_f>;
        using MsgResultType = typename MsgTypeTrait<MsgType>::ResultType;

        static_assert(std::is_same_v<InvokeResult, MsgResultType> ||
                      std::is_same_v<InvokeResult, Future<MsgResultType>>,
                      "return type mismatch");

        using Base = BehaviorBase<F, MsgType>;

    public:
        struct Type : detail::MsgHandler, private Base {
            using Base::Base;
            auto HandleMsg(Message& msg) noexcept -> bool override {
                return Base::HandleMsg(msg, [](MsgType& msg, F& f) -> InvokeResult {
                    return MsgTypeTrait<MsgType>::Call(msg, [&](auto &&... args) -> InvokeResult {
                        return f(AtomType{}, std::forward<decltype(args)>(args)...);
                    });
                });
            }
        };
    };

    template<typename F>
    struct BehaviorTrait<F, std::enable_if_t<!IS_ATOM_PATTERN<F>>> {
    private:
        static_assert((CallableTrait<F>::NUM_OF_ARGS == 1), "only message argument is allowed");
        using MsgType = std::decay_t<FirstArg<F>>;
        static_assert(!std::is_pointer_v<MsgType>, "don't use pointer, use reference instead");

        using Base = BehaviorBase<F, MsgType>;
    public:
        struct Type : detail::MsgHandler, private Base {
            using Base::Base;
            auto HandleMsg(Message& msg) noexcept -> bool override {
                return Base::HandleMsg(msg, [](MsgType &msg, F &f) { return f(msg); });
            }
        };
    };

    template<typename F>
    using BehaviorObject = typename BehaviorTrait<F>::Type;
}

#endif //NANO_CAF_2_142192B507274CADBFFCBEC979A70E95
