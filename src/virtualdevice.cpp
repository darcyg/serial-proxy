#include "virtualdevice.hpp"
#include "proxydevice.hpp"
#include "exception.hpp"

#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <errno.h>
#include <pty.h>

#include <stdexcept>
using namespace std;

#include <logcpp/logger.hpp>
using namespace logcpp;

VirtualDevice::VirtualDevice(const string &path, ProxyDevice *proxy)
:
SerialWrapper(path),
m_Slave(-1),
m_Writer(false),
m_Proxy(proxy)
{
}

VirtualDevice::~VirtualDevice(void)
{
    Close();
}

void VirtualDevice::Writer(bool writer)
{
    m_Writer = writer;
}

bool VirtualDevice::IsWriter(void) const
{
    return m_Writer;
}

bool VirtualDevice::Open(void)
{
    Log(LogDebug) << "Opening virtual: " << getPort();

    int master;
    if (openpty(&master, &m_Slave, NULL, NULL, NULL) == -1) {
        Log(LogCritical) << "openpty failed: " << strerror(errno);
        return false;
    }

    setFd(master);

    raw();

    remove(getPort().c_str());

    if (symlink(ttyname(m_Slave), getPort().c_str()) == -1) {
        Log(LogCritical) << "symlink failed: " << strerror(errno);
        return false;
    }

    return true;
}

void VirtualDevice::Close(void)
{
    Log(LogDebug) << "Closing virtual: " << getPort();

    remove(getPort().c_str());

    ::close(m_Slave);
}

void VirtualDevice::DataInReady(char* buffer, size_t len)
{
    if (IsWriter()) {
        Log(LogDebug) << "Virtual device writing: " << getPort() << " (" << len
            << ")";

        WriteToDevice(m_Proxy, buffer, len);
    }
}

string VirtualDevice::VirtualName(const string &device,
                                  const string &virtualName)
{
    return device + "." + virtualName;
}
