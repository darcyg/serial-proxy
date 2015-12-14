#include "configuration.hpp"
#include "utility.hpp"
#include "serial.hpp"
using namespace serial;

#include <algorithm>
using namespace std;

#include <logcpp/logger.hpp>
using namespace logcpp;

#include <inicpp/INIReader.hpp>
using namespace inicpp;

bool
Configuration::GetDirectory(vector<ConfigEntry> &config,
                            const string &path)
{
    Log(LogInformation) << "Configuration directory: " << path;

    if (!Utility::IsDirectory(path)) {
        Log(LogWarning) << "Configuration directory could not be opened";
        return false;
    }

    vector<string> configFiles = Utility::GetFiles(path, "*.ini");

    for (const auto &configFile : configFiles) {
        if (GetFile(config, configFile))
            Log(LogInformation) << "Configuration file: " << configFile;
    }

    return true;
}

bool
Configuration::GetFile(vector<ConfigEntry> &config,
                       const string &filename)
{
    INIReader reader(filename);

    if (reader.ParseError() < 0)
        return false;

    vector<string> devices = reader.GetSections();

    for (const auto &device : devices) {
        Log(LogDebug) << "Device: " << device;

        if (find_if(config.begin(), config.end(),
        [device](const ConfigEntry &item) {
            return device == item.device;
        })  != config.end()) {
            Log(LogWarning) << "Configuration for device '" << device
                << "' already exists";
            continue;
        }

        ConfigEntry item;

        item.device = device;
        item.virtuals = reader.GetList<string>(device, "virtuals");
        item.writer = reader.Get<string>(device, "writer", "");
        item.baudrate = reader.Get<int>(device, "baudrate", 9600);
        item.stopbits = ToStopBits(reader.Get<string>(device, "stopbits", "1"));
        item.bytesize = ToByteSize(reader.Get<string>(device, "bytesize", "8"));
        item.parity = ToParity(reader.Get<string>(device, "parity", "none"));
        item.flowcontrol = ToFlowcontrol(reader.Get<string>(device, "control",
            "none"));

        Log(LogDebug) << "Writer: " << item.writer;

        for (const auto &vsp : item.virtuals)
            Log(LogDebug) << "Virtual: " << vsp;

        config.push_back(item);
    }
    return true;
}

stopbits_t Configuration::ToStopBits(const string &bits)
{
    if (bits == "2") return stopbits_two;
    else if (bits == "1.5") return stopbits_one_point_five;
    return stopbits_one;
}

bytesize_t Configuration::ToByteSize(const string &size)
{
    if (size == "5") return fivebits;
    else if (size == "6") return sixbits;
    else if (size == "7") return sevenbits;
    return eightbits;
}

parity_t Configuration::ToParity(const string &parity)
{
    if (parity == "odd") return parity_odd;
    else if (parity == "even") return parity_even;
    else if (parity == "mark") return parity_mark;
    else if (parity == "space") return parity_space;
    return parity_none;
}

flowcontrol_t Configuration::ToFlowcontrol(const string &control)
{
    if (control == "software") return flowcontrol_software;
    else if (control == "hardware") return flowcontrol_hardware;
    return flowcontrol_none;
}
