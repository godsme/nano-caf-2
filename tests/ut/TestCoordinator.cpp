//
// Created by Darwin Yuan on 2022/6/15.
//
#include <nano-caf/scheduler/Coordinator.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    int ids[2] = {0, 0};
    std::size_t numOfTasks = 0;
    struct DummyTask : Resumable {
        DummyTask(int id) : id{id} {}
        auto Resume() noexcept -> void override {
            ids[id - 1] = id;
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

using namespace std::chrono_literals;

SCENARIO("Coordinator") {
    Coordinator coordinator{2};

    coordinator.Schedule(new DummyTask{1});
    coordinator.Schedule(new DummyTask{2});

    std::this_thread::sleep_for(100ms);

    REQUIRE(ids[0] + ids[1] == 3);
    REQUIRE(numOfTasks == 0);

    coordinator.Shutdown();
}