#include "logger.h"

Logger::Logger(LogLevel log_level): log_level(log_level) {
    this->log_file = "current_run.log";
    std::ofstream log_file (this->log_file, std::ios_base::app);
    if(log_file.is_open()) {
        log_file << "This is a line." << std::endl;
        log_file.close();
    }
}

int Logger::log_info(std::string message) {
    if(LogLevel::INFO <= this->log_level) {
        std::ofstream log_file (this->log_file, std::ios_base::app);
        if(log_file.is_open()) {
            log_file << "LOG [INFO]: ";
            log_file << message << std::endl;
            log_file.close();
        }
        return 0;
    }
    return 1;
}

int Logger::log_verbose(std::string message) {
    if(LogLevel::VERBOSE <= this->log_level) {
        std::ofstream log_file (this->log_file, std::ios_base::app);
        if(log_file.is_open()) {
            log_file << "LOG [VERBOSE]: ";
            log_file << message << std::endl;
            log_file.close();
        }
        return 0;
    }
    return 1;
}
