#include "serialwrapper.hpp"
#include "exception.hpp"

using namespace std;

#include <logcpp/logger.hpp>
using namespace logcpp;

SerialWrapper::SerialWrapper(const string &path)
:
Serial(),
m_WriteReady(false),
m_Error(false)
{
    setPort(path);
}

SerialWrapper::~SerialWrapper(void)
{
    Close();
}

void SerialWrapper::Error(void)
{
    m_Error = true;
}

bool SerialWrapper::IsError(void) const
{
    return m_Error;
}

void SerialWrapper::WriteReady(bool ready)
{
    m_WriteReady = ready;
}

bool SerialWrapper::IsWriteReady() const
{
    return m_WriteReady;
}

void SerialWrapper::WriteToDevice(SerialWrapper *device, char* buffer, size_t len)
{
    if (!device->IsWriteReady()) {
        Log(LogWarning) << "Device not ready: " << device->getPort();
        return;
    }

    if (!device->isOpen()) {
        Log(LogCritical) << "Device not open: " << device->getPort();
        Error();
        return;
    }

    Log(LogDebug) << "Writing to: " << device->getPort();

    int numWritten = device->write(buffer, len);

    if (numWritten < 0) {
        if (errno == EINTR)
            throw InterruptException("virtual write");

        Log(LogCritical) << "Write failed: " << strerror(errno);

        device->Error();
    } else if ((unsigned int) numWritten != len) {
        Log(LogWarning) << "Write failed: " << numWritten;
    }
}

bool SerialWrapper::Open(void)
{
    Log(LogDebug) << "Opening wrapper: " << getPort();

    try {
        open();
    } catch (const exception &e) {
        Log(LogCritical) << "Open failed: " << e.what();
        return false;
    }
    return true;
}

void SerialWrapper::Close(void)
{
    Log(LogDebug) << "Closing wrapper: " << getPort();

    try {
        serial::Serial::close();
    } catch (const exception &e) {
        Log(LogCritical) << "Closing failed: " << e.what();
    }
}