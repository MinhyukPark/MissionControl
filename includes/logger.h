#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

enum class LogLevel {
    SILENT = 0,
    INFO = 1,
    VERBOSE = 2,
};

class Logger {
    public:
        Logger(LogLevel log_level);
        LogLevel log_level;
        int log_info(std::string message);
        int log_verbose(std::string message);
};
#endif