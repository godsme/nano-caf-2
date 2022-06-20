//
// Created by Darwin Yuan on 2022/6/20.
//
#include <nano-caf/util/SharedPtr.h>
#include <catch.hpp>

using namespace nano_caf;

namespace {
    std::size_t numOfBlocks = 0;
    struct MemClaimer {
        static auto Alloc(std::size_t size) noexcept -> void* {
            numOfBlocks++;
            return ::operator new(size);
        }
        static auto Free(void* p) noexcept -> void {
            numOfBlocks--;
            ::operator delete(p);
        }
    };
}

SCENARIO("SharedPtr") {
    numOfBlocks = 0;
    {
        auto p = MakeShared<int, MemClaimer>(10);
        REQUIRE(numOfBlocks == 1);
        REQUIRE((bool)p);
        REQUIRE(*p == 10);
    }
    REQUIRE(numOfBlocks == 0);
}