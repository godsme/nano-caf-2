cmake_minimum_required(VERSION 3.22)
project(nano_caf_2)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

add_library(nano_caf
        include/nano-caf/scheduler/ListElem.h
        include/nano-caf/scheduler/Queue.h
        include/nano-caf/scheduler/Resumable.h
        include/nano-caf/scheduler/WorkSharingQueue.h
        include/nano-caf/scheduler/Coordinator.h
        src/scheduler/WorkSharingQueue.cpp
        src/scheduler/Worker.cpp
        src/scheduler/Coordinator.cpp)

add_executable(nano_caf_ut third_party/catch.cpp
        tests/ut/TestWorkSharingQueue.cpp
        tests/ut/TestWorker.cpp
        tests/ut/TestCoordinator.cpp)

target_link_libraries(nano_caf_ut nano_caf)
