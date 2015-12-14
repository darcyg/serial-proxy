#ifndef _PROXYDEVICE_HPP_
#define _PROXYDEVICE_HPP_

#include "seriallist.hpp"
#include "serialwrapper.hpp"
#include "virtualdevice.hpp"

class ProxyDevice : public SerialWrapper
{
public:
    ProxyDevice(const std::string &path);

    void DataInReady(char* buffer, size_t len) override;

    /**
     * Return list of child virtuals.
     *
     * @return List of virtual devices
     */
    SerialList<VirtualDevice>& Virtuals(void);

private:
    SerialList<VirtualDevice> m_Virtuals;
};

#endif
