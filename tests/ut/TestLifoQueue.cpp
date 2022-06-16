//
// Created by Darwin Yuan on 2022/6/16.
//
#include <nano-caf/actor/LifoQueue.h>
#include <nano-caf/message/Message.h>
#include <catch.hpp>

using namespace nano_caf;

SCENARIO("Lifo") {
    LifoQueue queue;

    queue.Enqueue(new Message{});
}