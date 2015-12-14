#ifndef _VIRTUALDEVICE_HPP_
#define _VIRTUALDEVICE_HPP_

#include "serialwrapper.hpp"

class ProxyDevice;

class VirtualDevice : public SerialWrapper
{
public:
    VirtualDevice(const std::string &path, ProxyDevice *proxy);
    ~VirtualDevice(void);

    bool Open(void) override;

    void Close(void) override;

    void DataInReady(char* buffer, size_t len) override;

    /**
     * Set writer status.
     *
     * @param writer True if writer, false otherwise
     */
    void Writer(bool writer);

    /**
     * Check if device is a writer or not.
     *
     * @return True if write, false otherwise
     */
    bool IsWriter(void) const;

    /**
     * Return virtual name based on parent device name.
     *
     * @param device Parent device
     * @param virtualName Virtual name suffix
     * @return Full path of virtual device name
     */
    static std::string VirtualName(const std::string &device,
        const std::string &virtualName);

private:
    int m_Slave;
    bool m_Writer;
    ProxyDevice *m_Proxy;
};

#endif
