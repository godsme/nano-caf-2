//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_142192B507274CADBFFCBEC979A70E95
#define NANO_CAF_2_142192B507274CADBFFCBEC979A70E95

#include <nano-caf/msg/Atom.h>
#include <nano-caf/util/CallableTrait.h>
#include <nano-caf/actor/detail/MsgTypeTrait.h>
#include <nano-caf/actor/detail/BehaviorBase.h>
#include <nano-caf/async/Future.h>

namespace nano_caf::detail {
    template<typename F, typename = void>
    struct BehaviorTrait;

    template<typename T>
    constexpr bool NonConstLref = !std::is_const_v<T> && std::is_lvalue_reference_v<T>;

    template<typename F>
    struct BehaviorTrait<F, std::enable_if_t<IS_MSG_ATOM<F>>> {
    private:
        static_assert(!NonConstLref<FirstArg<F>>, "the atom type cannot be non-const-lvalue-ref");
        using AtomType = std::decay_t<FirstArg<F>>;
        using MsgType = typename AtomType::MsgType;
        using FieldsTypes = typename MsgTypeTrait<MsgType>::FieldsTypes;

        template <typename ... Ts>
        using Invokable = std::is_invocable<F, AtomType, std::decay_t<Ts>...>;

        static_assert(FieldsTypes::template ExportTo<Invokable>::value, "parameters & message don't match");

        template <typename ... Ts>
        using InvokeResult_i = std::invoke_result_t<F, AtomType, Ts...>;
        using InvokeResult = typename FieldsTypes::template ExportTo<InvokeResult_i>;
        using MsgResultType = typename MsgTypeTrait<MsgType>::result_type;

        static_assert(std::is_same_v<InvokeResult, MsgResultType> ||
                      std::is_same_v<InvokeResult, Future<MsgResultType>>,
                      "return type mismatch");

        using Base = BehaviorBase<F, MsgType>;

    public:
        struct Type : Base {
            using Base::bBasease;
            auto operator()(Message &msg) noexcept -> bool {
                return Base::HandleMsg(msg, [](MsgType& msg, F& f) -> InvokeResult {
                    return MsgTypeTrait<MsgType>::Call(msg, [&](auto &&... args) -> InvokeResult {
                        return f(AtomType{}, std::move(args)...);
                    });
                });
            }
        };
    };

    template<typename F>
    struct BehaviorTrait<F, std::enable_if_t<!IS_MSG_ATOM<F>>> {
    private:
        static_assert((CallableTrait<F>::NUM_OF_ARGS == 1), "only message argument is allowed");
        using MsgType = std::decay_t<FirstArg<F>>;
        static_assert(!std::is_pointer_v<MsgType>, "don't use pointer, use reference instead");

        using Base = BehaviorBase<F, MsgType>;
    public:
        struct Type : Base {
            using Base::Base;

            auto operator()(Message &msg) -> bool {
                return Base::HandleMsg(msg, [](MsgType &msg, F &f) { return f(msg); });
            }
        };
    };

    template<typename F>
    using BehaviorObject = typename BehaviorTrait<F>::Type;
}

#endif //NANO_CAF_2_142192B507274CADBFFCBEC979A70E95
