//
// Created by Darwin Yuan on 2022/6/17.
//

#include <nano-caf/actor/SchedActor.h>
#include <nano-caf/msg/Message.h>
#include <nano-caf/msg/PredefinedMsgs.h>
#include <nano-caf/actor/detail/ActorCtlBlock.h>

namespace nano_caf {
    SchedActor::SchedActor(bool syncRequired) {
        if(syncRequired) {
            CtlBlock()->EnableSync();
        }
    }

    auto SchedActor::OnExit(ExitReason reason) noexcept -> void {
        ExitHandler(reason);
        CtlBlock()->OnExit(*m_exitReason);
    }

    SchedActor::~SchedActor() {
        if(!m_exitReason) {
            OnExit(ExitReason::UNKNOWN);
        }
    }

    auto SchedActor::ExitCheck() noexcept -> TaskResult {
        if(m_exitReason) {
            OnExit(*m_exitReason);
            return TaskResult::DONE;
        }
        return TaskResult::SUSPENDED;
    }

    auto SchedActor::Close(ExitReason reason) -> void {
        if(!m_exitReason) {
            m_exitReason.emplace(reason);
            OnExit(*m_exitReason);
        }
    }

    auto SchedActor::Init() noexcept -> TaskResult {
        InitHandler();
        return ExitCheck();
    }

    auto SchedActor::Resume(std::size_t maxSchedMsgs) noexcept -> TaskResult {
        if(!m_inited) {
            m_inited = true;
            if(auto result = Init(); result != TaskResult::SUSPENDED) {
                return result;
            }
        }

        auto result = MailBox::Consume(maxSchedMsgs, [this](std::unique_ptr<Message> msg) -> TaskResult {
            m_currentMsg = std::move(msg);
            HandleMsg(*m_currentMsg);
            m_currentMsg.reset();
            return ExitCheck();
        });

        return result;
    }

    auto SchedActor::Exit_(ExitReason reason) -> void {
        if(!m_exitReason) {
            m_exitReason.emplace(reason);
        }
    }

    auto SchedActor::HandleMsg(Message& msg) noexcept -> void {
        switch(msg.id) {
            case BootstrapMsg::ID: break; // ignore
            case FutureDoneNotify::ID: {
                auto notifier = msg.Body<FutureDoneNotify>()->notifier;
                notifier->Commit();
                break;
            }
            case TimeoutMsg::ID: {
                auto timeout = msg.template Body<TimeoutMsg>();
                if(!timeout->id.IsCancelled()) {
                    timeout->callback();
                }
                break;
            }
            default: {
                if(!HandleUserDefinedMsg(msg)) {
                    // This message didn't get processed.
                    msg.OnDiscard();
                }

                break;
            }
        }
    }

    auto SchedActor::CtlBlock() noexcept -> detail::ActorCtlBlock* {
        return reinterpret_cast<detail::ActorCtlBlock*>(reinterpret_cast<char*>(this) - CACHE_LINE_SIZE);
    }
}