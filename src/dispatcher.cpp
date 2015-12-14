#include "dispatcher.hpp"
#include "utility.hpp"
#include "configuration.hpp"
#include "exception.hpp"
#include "proxydevice.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <algorithm>
using namespace std;

#include <errno.h>
#include <unistd.h>

#include <logcpp/logger.hpp>
using namespace logcpp;

Dispatcher::Dispatcher(const string &configFile, size_t pollTimeout)
:
m_Reload(true),
m_Shutdown(false),
m_EpollFd(-1),
m_ConfigFile(configFile),
m_PollTimeout(pollTimeout)
{
    AddSignal(SIGINT);
    AddSignal(SIGTERM);
    AddSignal(SIGHUP);
}

Dispatcher::~Dispatcher(void)
{
    DoShutdown();
}

int Dispatcher::Run(void)
{
    try {
        if ((m_EpollFd = epoll_create1(0)) == -1)
            throw runtime_error("epoll_create failed");

        m_EpollEvents = (epoll_event*) calloc(MAX_EVENTS,
            sizeof(struct epoll_event));

        if (!m_EpollEvents)
            throw runtime_error("calloc(epoll_event*) failed");

        do {
            try  {

                DoReload();

                Poll();

                Clean();

                usleep(SLEEP_INTERVAL);

            } catch (const InterruptException &e) {
                Log(LogNotice) << "InterruptException: " << e.what();
            }
        } while(!m_Shutdown);

    } catch (const runtime_error &e) {
        Log(LogCritical) << "RuntimeError: " << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Dispatcher::Reload(void)
{
    m_Reload = true;
}

void Dispatcher::Shutdown(void)
{
    m_Shutdown = true;
}

void Dispatcher::Clean(void)
{
    SerialList<ProxyDevice>::iterator dIter  = m_Proxies.begin();

    while (dIter != m_Proxies.end()) {
        if ((*dIter)->IsError())
            dIter = m_Proxies.Remove(dIter);
        else {
            SerialList<VirtualDevice> &virtuals = (*dIter)->Virtuals();
            SerialList<VirtualDevice>::iterator vIter = virtuals.begin();

            while (vIter != virtuals.end()) {
                if ((*vIter)->IsError())
                    vIter = virtuals.Remove(vIter);
                else
                    ++vIter;
            }
            ++dIter;
        }
    }
}

void Dispatcher::Poll(void)
{
    BlockSignals();

    int ret = epoll_wait(m_EpollFd, m_EpollEvents, MAX_EVENTS, m_PollTimeout);

    if (ret == -1) {
        if (errno != EINTR)
            throw runtime_error("epoll_wait failed");
        /* We block signals, so this should not happen */
        else
            throw InterruptException("epoll");
    } else if (ret == 0) {
        Log(LogNotice) << "epoll timeout";
    } else {
        Log(LogDebug) << "#" << ret << " epoll events";

        /* Flag devices that are write-ready */
        for (int i = 0; i < ret; i++) {
            if (m_EpollEvents[i].events & EPOLLOUT) {
                SerialWrapper *s = (SerialWrapper*) m_EpollEvents[i].data.ptr;
                s->WriteReady(true);
            }
        }

        /* Get data from read-ready devices and write to write-ready devices */
        for (int i = 0; i < ret; i++) {
            if (m_EpollEvents[i].events & EPOLLIN) {
                SerialWrapper *s = (SerialWrapper*) m_EpollEvents[i].data.ptr;

                int numRead = s->read(m_Buffer, BUFSIZ);

                if (numRead < 0) {
                    if (errno == EINTR)
                        throw InterruptException("proxy read");

                    Log(LogCritical) << "Read failed: " << s->getPort()
                        << " (" << strerror(errno) << ")";

                    s->Error();

                    continue;
                }

                s->DataInReady(m_Buffer, numRead);
            }
        }

        /* Reset devices that were write-ready */
        for (int i = 0; i < ret; i++) {
            if (m_EpollEvents[i].events & EPOLLOUT) {
                SerialWrapper *s = (SerialWrapper*) m_EpollEvents[i].data.ptr;
                s->WriteReady(false);
            }
        }
    }

    UnblockSignals();
}

void Dispatcher::ReceiveSignal(int sigNum)
{
    Log(LogNotice) << "Program received signal: " << sigNum;

    switch(sigNum) {
        case SIGTERM:
        case SIGINT:
            Shutdown();
            break;
        case SIGHUP:
            Reload();
            break;
        default:
            break;
    }
}

void Dispatcher::Watch(int fd, void* data)
{
    static struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = fd;
    event.data.ptr = data;

    if (epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, fd, &event) == -1)
        throw runtime_error("epoll_ctl(...) failed");
}

void Dispatcher::DoReload(void)
{
    if (!m_Reload)
        return;

    m_Reload = false;

    Log(LogDebug) << "Reloading configuration";

    vector<ConfigEntry> config;

    if (!Configuration::GetDirectory(config, m_ConfigFile))
        return;

    for (auto& entry : config) {
        Log(LogDebug) << "Device: " << entry.device;

        ProxyDevice *p = m_Proxies.Get(entry.device);

        if (p) {
            bool reopen = false;

            if (p->getBaudrate() != entry.baudrate) {
                p->setBaudrate(entry.baudrate);
                reopen = true;
            }
            if (p->getBytesize() != entry.bytesize) {
                p->setBytesize(entry.bytesize);
                reopen = true;
            }
            if (p->getParity() != entry.parity) {
                p->setParity(entry.parity);
                reopen = true;
            }
            if (p->getStopbits() != entry.stopbits) {
                p->setStopbits(entry.stopbits);
                reopen = true;
            }
            if (p->getFlowcontrol() != entry.flowcontrol) {
                p->setFlowcontrol(entry.flowcontrol);
                reopen = true;
            }

            if (reopen) {
                Log(LogDebug) << "Reopening device";

                p->Close();

                if (!p->Open()) {
                    Log(LogCritical) << p->getPort()
                        << " failed to re-open after configuration changes";
                    delete p;
                    continue;
                }

                Watch(p->getFd(), p);
            }
        } else {
            p = new ProxyDevice(entry.device);
            p->setBaudrate(entry.baudrate);
            p->setBytesize(entry.bytesize);
            p->setParity(entry.parity);
            p->setStopbits(entry.stopbits);
            p->setFlowcontrol(entry.flowcontrol);

            if (!p->Open()) {
                Log(LogCritical) << p->getPort() << " failed to open";
                delete p;
                continue;
            }

            Watch(p->getFd(), p);

            m_Proxies.Add(p);
        }

        for (auto& virt : entry.virtuals) {
            virt = VirtualDevice::VirtualName(entry.device, virt);

            Log(LogDebug) << "Virtual: " << virt;

            if (!p->Virtuals().Exists(virt)) {
                VirtualDevice *v = new VirtualDevice(virt, p);

                if (!v->Open()) {
                    Log(LogCritical) << v->getPort() << " failed to open vsp";
                    delete v;
                    continue;
                }

                Watch(v->getFd(), v);

                p->Virtuals().Add(v);
            }
        }

        /* Remove virtuals & set writer */
        SerialList<VirtualDevice> &virtuals = p->Virtuals();
        SerialList<VirtualDevice>::iterator vIter  = virtuals.begin();

        while (vIter != virtuals.end()) {
            const string virtualName = (*vIter)->getPort();

            if (find(entry.virtuals.begin(), entry.virtuals.end(),
                virtualName) == entry.virtuals.end())
                vIter = virtuals.Remove(vIter);
            else {
                if (Utility::Extension(virtualName) == entry.writer) {
                    Log(LogDebug) << "Set writer: " << virtualName;
                    (*vIter)->Writer(true);
                } else
                    (*vIter)->Writer(false);
                ++vIter;
            }
        }
    }

    /* Remove devices */
    SerialList<ProxyDevice>::iterator dIter  = m_Proxies.begin();

    while (dIter != m_Proxies.end()) {
        if (find_if(config.begin(), config.end(),
        [dIter](const ConfigEntry &item) {
            return (*dIter)->getPort() == item.device;
        }) == config.end())
            dIter = m_Proxies.Remove(dIter);
        else
            ++dIter;
    }

    Log(LogDebug) << "Done Reloading configuration";
}

void Dispatcher::DoShutdown(void)
{
    Log(LogDebug) << "Shutting down";

    ::close(m_EpollFd);

    free(m_EpollEvents);
}

const size_t Dispatcher::MAX_EVENTS = 64;
const size_t Dispatcher::SLEEP_INTERVAL = 250000;
