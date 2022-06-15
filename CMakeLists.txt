cmake_minimum_required(VERSION 3.22)
project(nano_caf_2)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(nano_caf_2
        include/nano-caf/scheduler/list_elem.h
        include/nano-caf/scheduler/queue.h
        include/nano-caf/scheduler/resumable.h
        include/nano-caf/scheduler/work_sharing_queue.h
        src/scheduler/work_sharing_queue.cpp)
