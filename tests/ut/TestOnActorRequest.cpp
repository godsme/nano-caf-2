//
// Created by Darwin Yuan on 2022/6/24.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/msg/RequestDef.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    enum : uint32_t {
        MSG_interface_id = 1
    };

    __CAF_actor_interface(Msg, MSG_interface_id,
        (Open,  (const long&) -> long),
        (Close, (const long&) -> void)
    );

    struct MyActor : Actor {
        Status cause{Status::OK};
        auto Test() {
            ActorHandle to;
            Request<Msg::Open>(to, 100).Fail([this](Status cause) {
                this->cause = cause;
            });
        }
        auto HandleMessage(Message &msg) noexcept -> void {}
    };
}

SCENARIO("OnActorRequest") {
    detail::InternalActor<MyActor> actor{false};
    actor.Test();
    REQUIRE(actor.cause != Status::OK);
}