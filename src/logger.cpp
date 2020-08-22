#include "logger.h"

Logger::Logger(LogLevel log_level): log_level(log_level) {}

int Logger::log_info(std::string message) {
    if(LogLevel::INFO <= this->log_level) {
        std::cout<<"LOG [INFO]: "<<message<<std::endl;
        return 0;
    }
    return 1;
}

int Logger::log_verbose(std::string message) {
    if(LogLevel::VERBOSE <= this->log_level) {
        std::cout<<"LOG [VERBOSE]: "<<message<<std::endl;
        return 0;
    }
    return 1;
}
