//
// Created by Darwin Yuan on 2022/6/21.
//

#ifndef NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E
#define NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E

#include <nano-caf/util/WeakPtr.h>
#include <nano-caf/actor/AbstractActor.h>

namespace nano_caf {
    using ActorPtr = SharedPtr<AbstractActor>;
    using ActorWeakPtr = WeakPtr<AbstractActor>;
}

#endif //NANO_CAF_2_6362C872E1C14D48AD7844DE2970D61E
