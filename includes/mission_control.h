#ifndef MISSION_CONTROL_H
#define MISSION_CONTROL_H

#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "consts.h"
#include "logger.h"

class MissionControl {
    public:
        struct MetaExecutable {
            char* executable_path;
            char* executable_output;
            char* executable_error;
            char** executable_arguments;
            int executable_arguments_count;
            MetaExecutable(){
            }
            MetaExecutable(char* executable_path, char* executable_output, char* executable_error, char** executable_arguments, int executable_arguments_count):executable_path(executable_path), executable_output(executable_output), executable_error(executable_error), executable_arguments(executable_arguments), executable_arguments_count(executable_arguments_count) {
            };
            ~MetaExecutable(){
                clear();
            }
            void clear() {
                for(int i = 0; i < executable_arguments_count; i ++) {
                    delete [] executable_arguments[i];
                }
                delete [] executable_arguments;
                delete [] executable_path;
                delete [] executable_output;
                delete [] executable_error;
            }
        };

        std::map<std::string, std::string> executable_paths;
        std::map<std::string, std::string> executable_outputs;
        std::map<std::string, std::string> executable_errors;
        std::map<std::string, std::vector<std::string>>  executable_arguments;
        std::map<std::string, int>  executable_arguments_count;
        std::vector<std::string> executable_names;
        int executable_count;
        int parallelism;
        std::string current_run;
        Logger* logger;

        int init_mission_control();
        std::string format_output_file(std::string executable_name);
        std::string format_error_file(std::string executable_name);
        int test_mission_control();
        int list_executables();
        static void* program_launcher(void* args);
        int run_executable(pthread_t* worker_threads, MetaExecutable** meta_executables, int work_id);
        int run_executables();
        int add_executable(std::string executable_name, std::string executable_path);
        int remove_executable(std::string executable_name);
        int update_executable_argument(std::string executable_name, std::vector<std::string> executable_arguments);
};

#endif
