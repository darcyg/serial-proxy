#include "proxydevice.hpp"
#include "exception.hpp"

#include <cstring>
#include <errno.h>

using namespace std;

#include <logcpp/logger.hpp>
using namespace logcpp;

ProxyDevice::ProxyDevice(const string &path)
:
SerialWrapper(path)
{
}

void ProxyDevice::DataInReady(char* buffer, size_t len)
{
    Log(LogDebug) << "Device writing: " << getPort() << " (" << len << ")";

    for (auto *vsp : m_Virtuals) {
        WriteToDevice(vsp, buffer, len);
    }
}

SerialList<VirtualDevice>& ProxyDevice::Virtuals(void)
{
    return m_Virtuals;
}