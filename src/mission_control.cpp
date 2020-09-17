#include "mission_control.h"

int MissionControl::init_mission_control() {
    this->logger = new Logger(static_cast<LogLevel>(LOG_LEVEL));
    this->parallelism = std::max(1, std::min(MAX_PARALLELISM, parallelism));
    /* std::filesystem::create_directories(INPUT_DIR); */
    /* std::filesystem::create_directories(OUTPUT_DIR); */
    /* std::filesystem::create_directories(ERROR_DIR); */
    /* std::filesystem::create_directories(TEMP_WORK_DIR); */
    system(("mkdir -p " + INPUT_DIR).c_str());
    system(("mkdir -p " + OUTPUT_DIR).c_str());
    system(("mkdir -p " + ERROR_DIR).c_str());
    system(("mkdir -p " + TEMP_WORK_DIR).c_str());
    return 0;
}

int MissionControl::test_mission_control() {
    this->logger->log_info("Running with parallelism " + std::to_string(this->parallelism));
    this->logger->log_info("Input Directory read as  " + INPUT_DIR);
    this->logger->log_info("Output Directory read as  " + OUTPUT_DIR);
    this->logger->log_info("Error Directory read as  " + ERROR_DIR);
    this->logger->log_info("Temp Work Directory read as  " + TEMP_WORK_DIR);
    this->logger->log_info("Current Run Description: " + CURRENT_RUN_DESCRIPTION);
    return this->parallelism;
}

std::string MissionControl::format_output_file(std::string executable_name) {
    return OUTPUT_DIR + executable_name + ".out";
}

std::string MissionControl::format_error_file(std::string executable_name) {
    return ERROR_DIR + executable_name + ".err";
}

int MissionControl::list_executables() {
    for(int i = 0; i < this->executable_count; i ++) {
        this->logger->log_verbose("Executable " + std::to_string(i) + ": ");
        std::string current_executable_name = this->executable_names[i];
        this->logger->log_verbose(current_executable_name);

        this->logger->log_verbose("Executable Path:");
        this->logger->log_verbose(this->executable_paths[current_executable_name]);

        this->logger->log_verbose("Executable Output File: ");
        this->logger->log_verbose(this->executable_outputs[current_executable_name]);

        this->logger->log_verbose("Executable Error File: ");
        this->logger->log_verbose(this->executable_errors[current_executable_name]);

        this->logger->log_verbose("Current Argument Count: ");
        int current_argument_count = this->executable_arguments_count[current_executable_name];
        this->logger->log_verbose(std::to_string(current_argument_count));

        this->logger->log_verbose("Current Arguments: ");
        std::string current_arguments_str = this->executable_arguments[current_executable_name][0];
        for(int j = 1; j < current_argument_count; j ++) {
            current_arguments_str += (" " + this->executable_arguments[current_executable_name][j]);
        }
        this->logger->log_verbose(current_arguments_str);
    }
    return 0;
}

void* MissionControl::program_launcher(void* args) {
    MetaExecutable* current_executable = (MetaExecutable*)args;
    int current_pid = fork();
    if(current_pid == 0) {
        FILE* output_file = fopen(current_executable->executable_output, "w+");
        FILE* error_file = fopen(current_executable->executable_error, "w+");
        if(output_file == NULL) {
            perror("fopen executable output failed");
        }
        if(error_file == NULL) {
            perror("fopen executable error failed");
        }
        int output_fileno = fileno(output_file);
        int error_fileno = fileno(error_file);
        dup2(output_fileno, STDIN_FILENO);
        dup2(output_fileno, STDOUT_FILENO);
        dup2(error_fileno, STDERR_FILENO);
        fclose(output_file);
        fclose(error_file);
        // add if file exists for env/bin/activate, run it to source

        execvp(current_executable->executable_path, current_executable->executable_arguments);
    } else {
        wait(NULL);
    }
    return NULL;
}

int MissionControl::run_executable(pthread_t* worker_threads, MetaExecutable** meta_executables, int worker_id) {
    std::string current_executable = this->executable_names[worker_id];
    int current_executable_arguments_count = this->executable_arguments_count[current_executable];

    char* current_executable_path_ptr = new char[this->executable_paths[current_executable].length() + 1];
    std::strcpy(current_executable_path_ptr, this->executable_paths[current_executable].c_str());

    char* current_executable_output_ptr = new char[this->executable_outputs[current_executable].length() + 1];
    std::strcpy(current_executable_output_ptr, this->executable_outputs[current_executable].c_str());

    char* current_executable_error_ptr = new char[this->executable_errors[current_executable].length() + 1];
    std::strcpy(current_executable_error_ptr, this->executable_errors[current_executable].c_str());

    char** current_executable_arguments_ptr = new char*[current_executable_arguments_count + 1];
    for(int j = 0; j < current_executable_arguments_count; j ++) {
        current_executable_arguments_ptr[j] = new char[this->executable_arguments[current_executable][j].length() + 1];
        std::strcpy(current_executable_arguments_ptr[j], this->executable_arguments[current_executable][j].c_str());
    }
    current_executable_arguments_ptr[current_executable_arguments_count] = NULL;

    MetaExecutable* current_meta_executable = new MetaExecutable(current_executable_path_ptr, current_executable_output_ptr, current_executable_error_ptr, current_executable_arguments_ptr, current_executable_arguments_count);
    meta_executables[worker_id] = current_meta_executable;
    this->logger->log_info("calling: " + current_executable);
    pthread_create(&worker_threads[worker_id], NULL, MissionControl::program_launcher, (void*)current_meta_executable);
    return 0;
}

int MissionControl::run_executables() {
    pthread_t worker_threads[this->executable_count];
    MetaExecutable* meta_executables[this->executable_count];
    int start = 0;
    if(this->parallelism < this->executable_count) {
        int stride_end = (this->executable_count / this->parallelism) * this->parallelism;
        start = stride_end;
        for(int i = 0; i < stride_end; i += this->parallelism) {
            for(int j = i; j < i + this->parallelism; j ++) {
                this->run_executable(worker_threads, meta_executables, j);
            }
            this->logger->log_verbose("Waiting to join id " + std::to_string(i) + " through " + std::to_string(i + this->parallelism - 1));
            for(int j = i; j < i + this->parallelism; j ++) {
                pthread_join(worker_threads[j], NULL);
                delete meta_executables[j];
            }
        }
    }
    for(int i = start; i < this->executable_count; i ++) {
        this->run_executable(worker_threads, meta_executables, i);
    }
    for(int i = start; i < this->executable_count; i ++) {
        this->logger->log_verbose("Waiting to join id " + std::to_string(i) + " through " + std::to_string(this->executable_count - 1));
        pthread_join(worker_threads[i], NULL);
        delete meta_executables[i];
    }
    return 0;
}

int MissionControl::add_executable(std::string executable_name, std::string executable_path) {
    this->executable_paths[executable_name] = executable_path;
    this->executable_outputs[executable_name] = format_output_file(executable_name);
    this->executable_errors[executable_name] = format_error_file(executable_name);
    this->executable_names.push_back(executable_name);

    ++this->executable_count;
    return 0;
}

int MissionControl::remove_executable(std::string executable_name) {
    this->executable_paths.erase(executable_name);
    this->executable_outputs.erase(executable_name);
    this->executable_errors.erase(executable_name);

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
