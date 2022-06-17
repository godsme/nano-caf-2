//
// Created by Darwin Yuan on 2022/6/15.
//
#include <nano-caf/scheduler/Coordinator.h>
#include <catch.hpp>

using namespace nano_caf;
using namespace std::chrono_literals;

namespace {
    int ids[4] = {0};
    auto reset_ids()  {
        for(auto i=0; i<4; i++) {
            ids[i] = 0;
        }
    }
    std::size_t numOfTasks = 0;
    struct DummyTask : Resumable {
        DummyTask(int id) : id{id} {}
        auto Resume() noexcept -> TaskResult override {
            ids[id - 1] = id;
            std::this_thread::sleep_for(10ms);
            return TaskResult::DONE;
        }

        auto operator new(std::size_t size) noexcept -> void* {
            numOfTasks++;
            return ::operator new(size);
        }

        auto operator delete(void* p) noexcept -> void {
            numOfTasks--;
            ::operator delete(p);
        }

        int id{};
    };
}

SCENARIO("Coordinator") {
    reset_ids();
    Coordinator coordinator{2};

    coordinator.Schedule(new DummyTask{1});
    coordinator.Schedule(new DummyTask{2});

    std::this_thread::sleep_for(100ms);

    REQUIRE(ids[0] + ids[1] == 3);
    REQUIRE(numOfTasks == 0);

    coordinator.Shutdown();
}

SCENARIO("Coordinator process more") {
    reset_ids();
    Coordinator coordinator{2};

    coordinator.Schedule(new DummyTask{1});
    coordinator.Schedule(new DummyTask{2});
    coordinator.Schedule(new DummyTask{3});
    coordinator.Schedule(new DummyTask{4});

    std::this_thread::sleep_for(100ms);

    REQUIRE(ids[0] + ids[1] + ids[2] + ids[3] == 10);
    REQUIRE(numOfTasks == 0);

    coordinator.Shutdown();
}

SCENARIO("Coordinator shutdown cleanup") {
    reset_ids();
    Coordinator coordinator{2};

    coordinator.Schedule(new DummyTask{1});
    coordinator.Schedule(new DummyTask{2});
    coordinator.Schedule(new DummyTask{3});
    coordinator.Schedule(new DummyTask{4});

    std::this_thread::sleep_for(10ms);

    // REQUIRE(ids[0] + ids[1] + ids[2] + ids[3] == 3);

    coordinator.Shutdown();

    REQUIRE(numOfTasks == 0);
}