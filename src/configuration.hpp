#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#include "serial.hpp"

#include <string>
#include <map>
#include <vector>

struct ConfigEntry
{
    /* physical device ie. /dev/ttyS1 */
    std::string device;
    /* virtual names, not paths ie. myvirtual */
    std::vector<std::string> virtuals;
    /* virtual name of writer */
    std::string writer;
    /* serial settings */
    uint32_t baudrate;
    serial::bytesize_t bytesize;
    serial::parity_t parity;
    serial::stopbits_t stopbits;
    serial::flowcontrol_t flowcontrol;
};

/*
 * Configuration must ALWAYS return a vector of ConfigEntry
 */
class Configuration
{
public:
    /**
     * Get all configuration entries from a given directory. Not recursive but
     * does glob similar file extensions.
     *
     * @param config Reference to list of configuration entries
     * @param path Directory path
     * @return True if no errors occurred, false otherwise
     */
    static bool GetDirectory(std::vector<ConfigEntry> &config,
        const std::string &path);

    /**
     * Get all configuration entries from a given file.
     *
     * @param config Reference to list of configuration entries
     * @param filename Filename
     * @return True if no errors occurred, false otherwise
     */
    static bool GetFile(std::vector<ConfigEntry> &config,
        const std::string &filename);

private:
    /**
     * Serial attribute conversions
     */
    static serial::stopbits_t ToStopBits(const std::string &bits);
    static serial::bytesize_t ToByteSize(const std::string &size);
    static serial::parity_t ToParity(const std::string &parity);
    static serial::flowcontrol_t ToFlowcontrol(const std::string &control);

    Configuration();
    Configuration(const Configuration&);
};

#endif
