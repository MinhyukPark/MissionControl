#include "mission_control.h"

int MissionControl::test_mission_control() {
    return 5;
}

int MissionControl::list_executables() {
    for(int i = 0; i < this->executable_count; i ++) {
        std::cout<<"Executable "<<i<<": ";
        std::string current_executable_name = this->executable_names[i];
        std::cout<<current_executable_name<<std::endl;

        std::cout<<"Executable Path:";
        std::cout<<this->executable_paths[current_executable_name]<<std::endl;

        std::cout<<"Executable Output File: ";
        std::cout<<this->executable_outputs[current_executable_name]<<std::endl;

        std::cout<<"Current Argument Count: ";
        int current_argument_count = this->executable_arguments_count[current_executable_name];
        std::cout<<current_argument_count<<std::endl;

        std::cout<<"Current Arguments: ";
        std::cout<<this->executable_arguments[current_executable_name][0];
        for(int j = 1; j < current_argument_count; j ++) {
            std::cout<<", "<<this->executable_arguments[current_executable_name][j];
        }
        std::cout<<std::endl;
    }
    return 0;
}

void* MissionControl::program_launcher(void* args) {
    MetaExecutable* current_executable = (MetaExecutable*)args;
    int current_pid = fork();
    if(current_pid == 0) {
        FILE* output_file = fopen(current_executable->executable_output, "w+");
        if(output_file == NULL) {
            perror("fopen executable output failed");
        }
        int output_fileno = fileno(output_file);
        dup2(output_fileno, STDIN_FILENO);
        dup2(output_fileno, STDOUT_FILENO);
        dup2(output_fileno, STDERR_FILENO);
        fclose(output_file);
        execvp(current_executable->executable_path, current_executable->executable_arguments);
    } else {
        wait(NULL);
    }
    return NULL;
}

int MissionControl::run_executables() {
    pthread_t worker_threads[this->executable_count];
    MetaExecutable* meta_executables[this->executable_count];
    for(int i = 0; i < this->executable_count; i ++) {
        std::string current_executable = this->executable_names[i];
        int current_executable_arguments_count = this->executable_arguments_count[current_executable];

        char* current_executable_path_ptr = new char[this->executable_paths[current_executable].length() + 1];
        std::strcpy(current_executable_path_ptr, this->executable_paths[current_executable].c_str());

        char* current_executable_output_ptr = new char[this->executable_outputs[current_executable].length() + 1];
        std::strcpy(current_executable_output_ptr, this->executable_outputs[current_executable].c_str());

        char** current_executable_arguments_ptr = new char*[current_executable_arguments_count + 1];
        for(int j = 0; j < current_executable_arguments_count; j ++) {
            current_executable_arguments_ptr[j] = new char[this->executable_arguments[current_executable][j].length() + 1];
            std::strcpy(current_executable_arguments_ptr[j], this->executable_arguments[current_executable][j].c_str());
        }
        current_executable_arguments_ptr[current_executable_arguments_count] = NULL;

        MetaExecutable* current_meta_executable = new MetaExecutable(current_executable_path_ptr, current_executable_output_ptr, current_executable_arguments_ptr, current_executable_arguments_count);
        meta_executables[i] = current_meta_executable;
        pthread_create(&worker_threads[i], NULL, MissionControl::program_launcher, (void*)current_meta_executable);
    }
    for(int i = 0; i < this->executable_count; i ++) {
        pthread_join(worker_threads[i], NULL);
        delete meta_executables[i];
    }
    return 0;
}

int MissionControl::add_executable(std::string executable_name, std::string executable_path, std::string executable_output) {
    this->executable_paths[executable_name] = executable_path;
    this->executable_outputs[executable_name] = executable_output;
    this->executable_names.push_back(executable_name);

    ++this->executable_count;
    return 0;
}

int MissionControl::remove_executable(std::string executable_name) {
    this->executable_paths.erase(executable_name);
    this->executable_names.erase(std::remove(this->executable_names.begin(), this->executable_names.end(), executable_name), this->executable_names.end());

    this->executable_arguments.erase(executable_name);
    this->executable_arguments_count.erase(executable_name);

    --this->executable_count;
    return 0;
}

int MissionControl::update_executable_argument(std::string executable_name, std::vector<std::string> executable_args) {
    this->executable_arguments[executable_name] = executable_args;
    this->executable_arguments[executable_name].insert(this->executable_arguments[executable_name].begin(), this->executable_paths[executable_name]);
    this->executable_arguments_count[executable_name] = executable_args.size() + 1;
    return 0;
}
