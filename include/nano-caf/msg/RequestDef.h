//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_BB6FF278D65443C89D772C52256ADA42
#define NANO_CAF_2_BB6FF278D65443C89D772C52256ADA42

#include <nano-caf/util/CallableTrait.h>
#include <nano-caf/util/TypeList.h>
#include <nano-caf/msg/Atom.h>
#include <nano-caf/msg/MsgTypeId.h>
#include <maco/map.h>

namespace nano_caf::detail {
    template<typename ATOM, typename SIGNATURE>
    struct RequestType {
        RequestType() = delete;
        RequestType(const RequestType&) = delete;
        RequestType(RequestType&&) = delete;

        static_assert(!CallableTrait<SIGNATURE>::IS_VARIADIC, "request does not support variadic arguments");
        using MethodType  = ATOM;
        using ResultType  = typename CallableTrait<SIGNATURE>::ResultType;
        using ArgsType    = typename CallableTrait<SIGNATURE>::DecayedArgsType;
        using PatternType = typename ArgsType::template PrependType<ATOM>;
        using MsgType     = typename ArgsType::template ExportTo<std::tuple>;
    };
}

#define __CAF_method_name(x)      __MACO_1st   x
#define __CAF_method_signature(x) __MACO_rest  x

////////////////////////////////////////////////////////////////////////////////////////////////
#define __CAF_actor_method(n, x)                                        \
struct __CAF_method_name(x);                                            \
template <typename T> struct __SeCrEtE_method<n, T>                     \
   : nano_caf::detail::RequestType                                      \
           < __CAF_method_name(x), auto __CAF_method_signature(x)> {    \
   constexpr static auto Name() -> const char* {                        \
       return __MACO_stringify(__CAF_method_name(x));                   \
   }                                                                    \
};                                                                      \
struct __CAF_method_name(x) : nano_caf::AtomSignature {                 \
   using Type = __SeCrEtE_method<n, __SeCrEtE_tHiS_tYpe>;               \
   struct MsgType : Type::MsgType {                                     \
      using TupleParent = Type::MsgType;                                \
      using TupleParent::TupleParent;                                   \
      [[maybe_unused]]  constexpr static bool IS_REQUEST = true;        \
      using ResultType = Type::ResultType;                              \
      MsgType(Type::MsgType&& rhs) : Type::MsgType(std::move(rhs)) {}   \
      [[maybe_unused]]  constexpr static nano_caf::MsgTypeId ID =       \
           static_cast<nano_caf::MsgTypeId>(n) | __secrete_type_id;     \
   };                                                                   \
};

///////////////////////////////////////////////////////////////////////////////////////////////
#define __CAF_actor_interface(name, i_type_id, ...)                                   \
struct name {                                                                         \
private:                                                                              \
   using __SeCrEtE_tHiS_tYpe = name;                                                  \
   static_assert(sizeof(i_type_id) == sizeof(uint32_t) &&                             \
                 std::is_convertible_v<decltype(i_type_id), uint32_t>,                \
                 "interface type id must be uint32_t");                               \
   constexpr static nano_caf::MsgTypeId __secrete_type_id =                           \
           static_cast<nano_caf::MsgTypeId>(i_type_id) << 32;                         \
public:                                                                               \
   template <size_t, typename> struct __SeCrEtE_method;                               \
   [[maybe_unused]] constexpr static size_t TOTAL_METHODS = __MACO_num_of_args(__VA_ARGS__); \
   __MACO_map_i(__CAF_actor_method, __VA_ARGS__);                                     \
}



#endif //NANO_CAF_2_BB6FF278D65443C89D772C52256ADA42
