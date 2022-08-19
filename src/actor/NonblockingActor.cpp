//
// Created by Darwin Yuan on 2022/7/1.
//

#include <nano-caf/actor/NonblockingActor.h>
#include <nano-caf/scheduler/ActorSystem.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>

namespace nano_caf {
    auto NonblockingActor::Send(Message* msg) noexcept -> Status {
        if(auto status = MailBox::SendMsg(msg); status != Status::BLOCKED) {
            return status;
        }
        return ActorSystem::Instance().Schedule(this);
    }

    namespace {
        constexpr std::size_t MAX_CONSUMED_MSGS_PER_ROUND = 3;
    }

    auto NonblockingActor::Resume() noexcept -> TaskResult {
        return SchedActor::Resume(MAX_CONSUMED_MSGS_PER_ROUND);
    }

    auto NonblockingActor::AddRef() noexcept -> void {
        return SchedActor::CtlBlock()->AddRef();
    }

    auto NonblockingActor::Release() noexcept -> void {
        return SchedActor::CtlBlock()->Release();
    }
}