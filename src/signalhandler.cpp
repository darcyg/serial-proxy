#include "signalhandler.hpp"

#include <stdexcept>
#include <cstring>
using namespace std;

#include <logcpp/logger.hpp>
using namespace logcpp;

SignalHandlerManagerPtr SignalHandlerManager::SigManager;

SignalHandlerManager::SignalHandlerManager(void)
{
}

SignalHandlerManager::SignalHandlerManager(const SignalHandlerManager&)
{
}

SignalHandlerManager&
SignalHandlerManager::operator=(const SignalHandlerManager&)
{
    return *this;
}

SignalHandlerManagerPtr SignalHandlerManager::GetInstance(void)
{
    if(!SigManager)
        SigManager.reset(new SignalHandlerManager);
    return SigManager;
}

void SignalHandlerManager::RegisterHandler(int signum, SignalHandler* handler)
{

    struct sigaction action;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SignalHandlerManager::DispatchSignal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if(sigaction(signum, &action, NULL) < 0)
        throw runtime_error("sigaction failed");

    m_Objects[signum] = handler;
}

void SignalHandlerManager::BlockAll(void)
{
    static sigset_t mask;

    sigfillset(&mask);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        throw runtime_error("sigprocmask(SIG_BLOCK) failed");
}

void SignalHandlerManager::UnblockAll(void)
{
    static sigset_t mask;

    sigfillset(&mask);

    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
        throw runtime_error("sigprocmask(SIG_BLOCK) failed");
}

void SignalHandlerManager::DispatchSignal(int signum)
{
    (GetInstance()->m_Objects[signum])->ReceiveSignal(signum);
}

SignalHandler::~SignalHandler(void)
{
}

void SignalHandler::AddSignal(int signum)
{
    (SignalHandlerManager::GetInstance())->RegisterHandler(signum, this);
}

void SignalHandler::BlockSignals(void)
{
    (SignalHandlerManager::GetInstance())->BlockAll();
}

void SignalHandler::UnblockSignals(void)
{
    (SignalHandlerManager::GetInstance())->UnblockAll();
}