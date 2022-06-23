//
// Created by Darwin Yuan on 2022/6/23.
//

#ifndef NANO_CAF_2_5BED94F2461540B1874839CCF81FB1A7
#define NANO_CAF_2_5BED94F2461540B1874839CCF81FB1A7

#include <nano-caf/actor/Actor.h>
#include <nano-caf/actor/Behavior.h>
#include <iostream>

namespace nano_caf {
    struct BehaviorBasedActor : Actor {
        auto HandleMessage(Message& msg) noexcept -> void {
            if(!m_behavior) {
                m_behavior = GetBehavior();
            }
            m_behavior.HandleMsg(msg);
        }

    protected:
        auto ChangeBehavior(Behavior const& to) noexcept -> void {
            if(to) m_behavior = to;
        }

    private:
        virtual auto GetBehavior() noexcept -> Behavior = 0;

    private:
        Behavior m_behavior;
    };
}

#endif //NANO_CAF_2_5BED94F2461540B1874839CCF81FB1A7
