#ifndef MISSION_CONTROL_H
#define MISSION_CONTROL_H

#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class MissionControl {
    public:
        std::map<std::string, std::string> executable_paths;
        std::map<std::string, std::vector<std::string>>  executable_arguments;
        std::map<std::string, int>  executable_arguments_count;
        std::vector<std::string> executable_names;
        int executable_count;
        int test_mission_control();
        int list_executables();
        static void* program_launcher(void* args);
        int run_executables();
        int add_executable(std::string executable_name, std::string executable_path);
        int remove_executable(std::string executable_name);
        int update_executable_argument(std::string executable_name, std::vector<std::string> executable_arguments);
        struct MetaExecutable {
            char* executable_path;
            char** executable_arguments;
            int executable_arguments_count;
            MetaExecutable(){
            }
            MetaExecutable(char* executable_path, char** executable_arguments, int executable_arguments_count):executable_path(executable_path), executable_arguments(executable_arguments), executable_arguments_count(executable_arguments_count) {
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
            }
        };
};

#endif
