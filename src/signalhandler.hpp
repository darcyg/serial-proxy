#ifndef _SIGNALHANDLER_HPP_
#define _SIGNALHANDLER_HPP_

#include <signal.h>
#include <map>
#include <memory>

class SignalHandler;
class SignalHandlerManager;

using SignalHandlerManagerPtr = std::shared_ptr<SignalHandlerManager>;

class SignalHandlerManager
{
private:
    SignalHandlerManager(void);
    SignalHandlerManager(const SignalHandlerManager&);
    SignalHandlerManager& operator=(const SignalHandlerManager&);

public:
    static SignalHandlerManagerPtr GetInstance(void);
    void RegisterHandler(int signum, SignalHandler* handler);
    void BlockAll(void);
    void UnblockAll(void);

private:
    static void DispatchSignal(int signum);

    static SignalHandlerManagerPtr SigManager;
    std::map<int, SignalHandler*> m_Objects;
};

class SignalHandler
{
public:
    virtual ~SignalHandler(void);

protected:
    friend class SignalHandlerManager;
    void AddSignal(int signum);
    void BlockSignals(void);
    void UnblockSignals(void);

    virtual void ReceiveSignal(int signum) = 0;
};

#endif