#ifndef _DISPATCHER_HPP_
#define _DISPATCHER_HPP_

#include "signalhandler.hpp"
#include "seriallist.hpp"

#include <string>
#include <cstdio>

#include <sys/epoll.h>

class ProxyDevice;

class Dispatcher : public SignalHandler
{
public:
    Dispatcher(const std::string &configFile, size_t pollTimeout);
    ~Dispatcher(void);

    /**
     * Dispatcher entry point. Blocks while reading and writing to serial ports.
     *
     * @return Status code on exit/termination
     */
    int Run(void);

    /**
     * Reload serial port configuration. Invoked by a SIGHUP signal.
     */
    void Reload(void);

    /**
     * Attempt a clean shutdown. Invoked by a SIGINT/SIGTERM signal.
     */
    void Shutdown(void);

protected:
    /**
     * Receive signal from underlying handler.
     *
     * @param sigNum Signal number received
     */
    void ReceiveSignal(int sigNum);

private:
    const static size_t MAX_EVENTS,
        SLEEP_INTERVAL;

    /**
     * Self signal to reload configuration.
     */
    void DoReload(void);

    /**
     * Self signal to shutdown.
     */
    void DoShutdown(void);

    /**
     * Process epoll events for activity.
     */
    void Poll(void);

    /**
     * Add a file descriptor to watch for read/write events.
     *
     * @param fd File descriptor to watch
     * @param data Void pointer to serial device to process events
     */
    void Watch(int fd, void* data);

    /**
     * Cleanup devices that have errors or have been closed.
     */
    void Clean(void);

    bool m_Reload, m_Shutdown;
    struct epoll_event *m_EpollEvents;
    int m_EpollFd;
    std::string m_ConfigFile;
    size_t m_PollTimeout;
    SerialList<ProxyDevice> m_Proxies;
    char m_Buffer[BUFSIZ];
};

#endif
