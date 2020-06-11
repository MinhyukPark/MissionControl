#include <iostream>
#include "library.h"
#include "mission_control.h"

int main(int argc, char* argv[]) {
    std::cout<<"Started Main"<<std::endl;
    MissionControl* mc = new MissionControl();
    std::string mafft_dummy_path = "/home/minhyuk2/local/bin/MAFFT_DUMMY";
    std::vector<std::string> mafft_dummy_args = {"arg1", "arg2"};
    mc->add_executable("mafft", mafft_dummy_path);
    mc->update_executable_argument("mafft", mafft_dummy_args);
    mc->list_executables();
    mc->run_executables();
    delete mc;
}
