cmake_minimum_required(VERSION 3.22)
project(nano_caf_2)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../maco/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

add_library(nano_caf
        include/nano-caf/Status.h
        include/nano-caf/util/TypeList.h
        include/nano-caf/util/MacroReflex.h
        include/nano-caf/util/ListElem.h
        include/nano-caf/util/Queue.h
        include/nano-caf/util/Queue.tcc
        include/nano-caf/util/SharedPtrDataHolder.h
        include/nano-caf/util/SharedPtr.h
        include/nano-caf/util/WeakPtr.h
        include/nano-caf/util/CacheLineSize.h
        include/nano-caf/util/SharedPtrCtlBlock.h
        include/nano-caf/util/SharedBlock.h
        include/nano-caf/util/DefaultMemAllocator.h
        include/nano-caf/util/Void.h
        include/nano-caf/util/CallableTrait.h
        include/nano-caf/util/TupleConcept.h
        include/nano-caf/util/AggregateFieldsType.h
        include/nano-caf/util/AggregateTrait.h
        include/nano-caf/scheduler/Resumable.h
        include/nano-caf/scheduler/WorkSharingQueue.h
        include/nano-caf/scheduler/Coordinator.h
        include/nano-caf/scheduler/TaskResult.h
        include/nano-caf/actor/detail/MsgHandler.h
        include/nano-caf/actor/detail/BehaviorBase.h
        include/nano-caf/actor/detail/Behaviors.h
        include/nano-caf/actor/detail/MsgTypeTrait.h
        include/nano-caf/actor/detail/BehaviorObject.h
        include/nano-caf/actor/LifoQueue.h
        include/nano-caf/actor/SchedActor.h
        include/nano-caf/actor/ActorPtr.h
        include/nano-caf/actor/Spawn.h
        include/nano-caf/actor/MailBox.h
        include/nano-caf/actor/ExitReason.h
        include/nano-caf/actor/ActorHandle.h
        include/nano-caf/actor/Behavior.h
        include/nano-caf/actor/Actor.h
        include/nano-caf/async/FutureConcept.h
        include/nano-caf/async/AbstractPromise.h
        include/nano-caf/async/detail/InvokeResultType.h
        include/nano-caf/async/detail/FutureObject.h
        include/nano-caf/async/detail/FutureObserver.h
        include/nano-caf/async/detail/FutureCallbackObject.h
        include/nano-caf/async/detail/FutureCallbackProxy.h
        include/nano-caf/async/Future.h
        include/nano-caf/async/Promise.h
        include/nano-caf/msg/Message.h
        include/nano-caf/msg/MakeMessage.h
        include/nano-caf/msg/MsgDef.h
        include/nano-caf/msg/RequestDef.h
        include/nano-caf/msg/RequestConcept.h
        src/scheduler/WorkSharingQueue.cpp
        src/scheduler/Worker.cpp
        src/scheduler/Coordinator.cpp
        src/scheduler/ActorSystem.cpp
        src/actor/LifoQueue.cpp
        src/actor/MailBox.cpp
        src/actor/SchedActor.cpp
        src/actor/ActorHandle.cpp
        src/util/SharedPtrCtlBlock.cpp)

add_executable(nano_caf_ut third_party/catch.cpp
        tests/ut/TestSharedPtr.cpp
        tests/ut/TestWorkSharingQueue.cpp
        tests/ut/TestWorker.cpp
        tests/ut/TestActor.cpp
        tests/ut/TestCoordinator.cpp
        tests/ut/TestLifoQueue.cpp
        tests/ut/TestSchedActor.cpp
        tests/ut/TestFutureObject.cpp
        tests/ut/TestFutureCallbackObject.cpp
        tests/ut/TestFuture.cpp
        tests/ut/TestBehaviorObject.cpp
        )

target_link_libraries(nano_caf_ut nano_caf)
