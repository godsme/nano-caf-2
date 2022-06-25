//
// Created by Darwin Yuan on 2022/6/24.
//
#include <nano-caf/actor/Spawn.h>
#include <nano-caf/actor/Actor.h>
#include <nano-caf/msg/RequestDef.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    enum : uint32_t {
        MSG_interface_id = 1
    };

    __CAF_actor_interface(Msg, MSG_interface_id,
                          (Open,(const long&) -> long),
                          (Close,(const long&) -> void)
    );

    CAF_def_message(DoneNotify, (num, long));

}

namespace {
    struct ServerActor : Actor {
        long base{0};
        ServerActor(long base) : base{base} {}

        auto GetBehavior() noexcept -> Behavior {
            return {
                [this](Msg::Open, long value) -> Future<long> {
                    return ExpectMsg<DoneNotify>([this, value](auto&& notify) -> long {
                        return base + value + notify.num;
                    });
                },
                [this](ExitMsg::Atom, ExitReason reason) {
                    Exit(reason);
                }
            };
        }
    };

    long result = 0;
    struct RequestActor : Actor {
        ActorHandle server{};
        long value{};

        RequestActor(long value, ActorHandle server)
            : server{server}
            , value{value}
        {}

        auto OnInit() -> void {
            Request<Msg::Open>(server, value)
                .Then([this](long r) {
                    result = r;
                    Exit(ExitReason::NORMAL);
                })
                .Fail([this](Status cause) {
                    Exit(ExitReason::ABNORMAL);
                });
        }
    };
}

SCENARIO("On Actor Expect Msg") {
    ActorSystem::Instance().StartUp(1);

    result = 0;

    auto server = Spawn<ServerActor>(101);
    REQUIRE(server);

    auto requester = Spawn<RequestActor, true>(203, server);
    REQUIRE(requester);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    server.Send<DoneNotify>(20);
    server.Release();

    ExitReason reason{ExitReason::UNKNOWN};
    REQUIRE(requester.Wait(reason) == Status::OK);
    REQUIRE(reason == ExitReason::NORMAL);
    REQUIRE(result == 203 + 101 + 20);

    ActorSystem::Instance().Shutdown();
}