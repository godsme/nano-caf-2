cmake_minimum_required(VERSION 3.22)
project(nano_caf_2)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(nano_caf_2
        include/nano-caf/scheduler/ListElem.h
        include/nano-caf/scheduler/Queue.h
        include/nano-caf/scheduler/Resumable.h
        include/nano-caf/scheduler/WorkSharingQueue.h
        src/scheduler/WorkSharingQueue.cpp)
