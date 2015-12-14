#include "dispatcher.hpp"
#include "utility.hpp"

#include <iostream>
#include <string>
using namespace std;

#include <logcpp/logger.hpp>
#include <logcpp/helpers/exception.hpp>
#include <logcpp/logger/filerotatelogger.hpp>
#include <logcpp/logger/consolelogger.hpp>
using namespace logcpp;
using namespace logcpp::helpers;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc;
    po::positional_options_description p;
    po::variables_map vm;

    desc.add_options()
        ("config,c",    po::value<string>(), "Configuration directory")
        ("logfile,f",   po::value<string>(), "Print log message to file")
        ("loglevel,l",  po::value<string>()->default_value("critical"), "Logging level")
        ("pidfile,p",   po::value<string>(), "Daemon pid file path")
        ("timeout,t",   po::value<int>()->default_value(1000), "Polling timeout")
        ("daemonize,d", "Run as a daemon")
        ("console,o",   "Print log messages to screen")
        ("help,h",      "Help message")
    ;

    try {

        po::store(po::command_line_parser(argc, argv).options(desc).run(),vm);

        po::notify(vm);

    } catch(const po::error &e) {
        cerr << "ParserError: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        cout << endl
             << "sproxyd - Serial port proxy daemon" << endl << endl
             << "USAGE" << endl << endl
             << "\tsproxyd [OPTIONS]" << endl << endl
             << "OPTIONS" << endl << endl
             << desc << endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("daemonize")) {
        if (!vm.count("pidfile")) {
            cerr << "ParserError: 'pidfile' is required when daemonize supplied."
                << endl;
            return EXIT_FAILURE;
        }

        Utility::Daemonize(vm["pidfile"].as<string>());
    }


    if (!vm.count("config")) {
        cerr << "ParserError: 'config' is required" << endl;
        return EXIT_FAILURE;
    }

    LogSeverity level;

    try {
        level = Logger::StringToSeverity(vm["loglevel"].as<string>());
    } catch (const LogcppException &e) {
        cerr << "LoggerError: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (vm.count("logfile")) {
        FileRotateLogger::Ptr logger =
            FileRotateLogger::Ptr(new FileRotateLogger());
        logger->SetPath(vm["logfile"].as<string>());
        logger->SetMaxBackup(5);
        logger->SetMaxSize(25000);
        logger->SetAppend(true);
        logger->SetSeverity(level);
        Logger::AddLogger(logger);
    }

    if (vm.count("console")) {
        ConsoleLogger::Ptr logger = ConsoleLogger::Ptr(new ConsoleLogger());
        logger->SetSeverity(level);
        Logger::AddLogger(logger);
    }

    return Dispatcher(vm["config"].as<string>(), vm["timeout"].as<int>()).Run();
}
