#ifndef _SERIALWRAPPER_HPP_
#define _SERIALWRAPPER_HPP_

#include "serial.hpp"

#include <string>

class SerialWrapper : public serial::Serial
{
public:
    SerialWrapper(const std::string &path);
    virtual ~SerialWrapper(void);

    /**
     * Invoked when data is available on the serial device.
     *
     * @param buffer Bytes
     * @param len Number of bytes to read
     */
    virtual void DataInReady(char* buffer, size_t len) = 0;

    /**
     * Set device write ready status.
     *
     * @param ready If device is ready to write
     */
    void WriteReady(bool ready);

    /**
     * Set device error status.
     */
    void Error(void);

    /**
     * Check if serial device has an error.
     *
     * @return True if has error, false otherwise
     */
    bool IsError(void) const;

    /**
     * Check if serial device is ready to write.
     *
     * @return True if device is ready to write, false otherwise
     */
    bool IsWriteReady(void) const;

    /**
     * Open serial device.
     *
     * @return True if no errors, false otherwise
     */
    virtual bool Open(void);

    /**
     * Close serial device.
     */
    virtual void Close(void);

protected:
    /**
     * Write to device if device is ready and has no errors.
     *
     * @param device Device to write to
     * @param buffer Bytes
     * @param len Number of bytes
     */
    void WriteToDevice(SerialWrapper *device, char* buffer, size_t len);

    bool m_WriteReady, m_Error;
};

#endif
