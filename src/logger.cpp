#include "logger.h"

Logger::Logger(LogLevel log_level): log_level(log_level) {
    this->log_file = "current_run.log";
    std::ofstream log_file (this->log_file, std::ios_base::app);
    if(log_file.is_open()) {
        std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
        std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);
        struct tm* current_tm_struct;
        char current_tm_buffer[80];
        current_tm_struct = localtime(&current_time_t);
        strftime(current_tm_buffer, sizeof(current_tm_buffer), "%Y-%m-%d_%H-%M-%S", current_tm_struct);
        std::string current_datetime(current_tm_buffer);
        log_file << "File created at:";
        log_file << current_datetime << std::endl;
        log_file.close();
    }
}


int Logger::log_info(std::string message) {
    if(LogLevel::INFO <= this->log_level) {
        std::ofstream log_file (this->log_file, std::ios_base::app);
        if(log_file.is_open()) {
            std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
            std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);
            struct tm* current_tm_struct;
            char current_tm_buffer[80];
            current_tm_struct = localtime(&current_time_t);
            strftime(current_tm_buffer, sizeof(current_tm_buffer), "%Y-%m-%d_%H-%M-%S", current_tm_struct);
            std::string current_datetime(current_tm_buffer);
            log_file << current_datetime;
            log_file << " LOG [INFO]: ";
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
            std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
            std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);
            struct tm* current_tm_struct;
            char current_tm_buffer[80];
            current_tm_struct = localtime(&current_time_t);
            strftime(current_tm_buffer, sizeof(current_tm_buffer), "%Y-%m-%d_%H-%M-%S", current_tm_struct);
            std::string current_datetime(current_tm_buffer);
            log_file << current_datetime;
            log_file << " LOG [VERBOSE]: ";
            log_file << message << std::endl;
            log_file.close();
        }
        return 0;
    }
    return 1;
}
