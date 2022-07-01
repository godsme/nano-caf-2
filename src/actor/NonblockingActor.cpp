//
// Created by Darwin Yuan on 2022/7/1.
//

#include <nano-caf/actor/NonblockingActor.h>
#include <nano-caf/scheduler/ActorSystem.h>

namespace nano_caf {
    auto NonblockingActor::SendMsg(Message* msg) noexcept -> Status {
        if(auto status = SchedActor::SendMsg(msg); status != Status::BLOCKED) {
            return status;
        }
        return ActorSystem::Instance().Schedule(this);
    }

    auto NonblockingActor::Wait(ExitReason& reason) noexcept -> Status {
        return SchedActor::Wait(reason);
    }

    namespace {
        constexpr std::size_t MAX_CONSUMED_MSGS_PER_ROUND = 3;
    }

    auto NonblockingActor::Resume() noexcept -> TaskResult {
        return SchedActor::Resume(MAX_CONSUMED_MSGS_PER_ROUND);
    }

    auto NonblockingActor::AddRef() noexcept -> void {
        return CtlBlock()->AddRef();
    }

    auto NonblockingActor::Release() noexcept -> void {
        return CtlBlock()->Release();
    }

    auto NonblockingActor::CtlBlock() noexcept -> SharedPtrCtlBlock* {
        return reinterpret_cast<SharedPtrCtlBlock*>(reinterpret_cast<char*>(this) - CACHE_LINE_SIZE);
    }
}