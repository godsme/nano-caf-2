//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_4A641C3A5498473C83E9472E32FB337B
#define NANO_CAF_2_4A641C3A5498473C83E9472E32FB337B

#include <nano-caf/msg/Atom.h>
#include <nano-caf/util/MacroReflex.h>
#include <nano-caf/msg/MsgTypeId.h>

///////////////////////////////////////////////////////////////////////////////////////////////
#define __CAF_def_message(name, id, ...)           \
struct name;                                       \
struct name##_atom : nano_caf::AtomSignature {     \
    constexpr static nano_caf::MsgTypeId ID = id;  \
    using MsgType = name;                          \
};                                                 \
struct name {                                      \
   constexpr static nano_caf::MsgTypeId ID = id;   \
   __REFLEX_fields_only(__VA_ARGS__)               \
}

///////////////////////////////////////////////////////////////////////////////////////////////
#define CAF_def_message(name, ...) __CAF_def_message(name, __COUNTER__, ##__VA_ARGS__)

#endif //NANO_CAF_2_4A641C3A5498473C83E9472E32FB337B
