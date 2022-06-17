cmake_minimum_required(VERSION 3.22)
project(nano_caf_2)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

add_library(nano_caf
        include/nano-caf/util/ListElem.h
        include/nano-caf/util/Queue.h
        include/nano-caf/scheduler/Resumable.h
        include/nano-caf/scheduler/WorkSharingQueue.h
        include/nano-caf/scheduler/Coordinator.h
        include/nano-caf/scheduler/TaskResult.h
        include/nano-caf/actor/LifoQueue.h
        include/nano-caf/actor/SchedActor.h
        include/nano-caf/actor/MailBox.h
        include/nano-caf/actor/ExitReason.h
        include/nano-caf/util/SharedPtr.h
        include/nano-caf/util/CacheLineSize.h
        include/nano-caf/util/SharedPtrCtlBlock.h
        include/nano-caf/util/SharedBlock.h
        include/nano-caf/util/DefaultMemClaimer.h
        include/nano-caf/message/Message.h
        src/scheduler/WorkSharingQueue.cpp
        src/scheduler/Worker.cpp
        src/scheduler/Coordinator.cpp
        src/actor/LifoQueue.cpp
        src/actor/MailBox.cpp
        src/actor/SchedActor.cpp
        src/util/SharedPtrCtlBlock.cpp)

add_executable(nano_caf_ut third_party/catch.cpp
        tests/ut/TestWorkSharingQueue.cpp
        tests/ut/TestWorker.cpp
        tests/ut/TestCoordinator.cpp
        tests/ut/TestLifoQueue.cpp
        tests/ut/TestSchedActor.cpp)

target_link_libraries(nano_caf_ut nano_caf)
