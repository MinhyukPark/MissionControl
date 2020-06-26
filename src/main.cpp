#include <iostream>
#include "library.h"
#include "mission_control.h"

int main(int argc, char* argv[]) {
    std::cout<<"Started Main"<<std::endl;
    MissionControl* mc = new MissionControl();
    std::string mafft_dummy_path = "/home/minhyuk2/local/bin/MAFFT_DUMMY";
    std::vector<std::string> mafft1_dummy_args = {"arg1", "arg2"};
    std::vector<std::string> mafft2_dummy_args = {"arg22", "arg44"};
    mc->add_executable("mafft1", mafft_dummy_path, "/scratch/users/minhyuk2/output/mafft_1.out");
    mc->add_executable("mafft2", mafft_dummy_path, "/scratch/users/minhyuk2/output/mafft_2.out");
    mc->update_executable_argument("mafft1", mafft1_dummy_args);
    mc->update_executable_argument("mafft2", mafft2_dummy_args);
    mc->list_executables();
    mc->run_executables();
    std::cout<<"end experiments"<<std::endl;
    delete mc;
}
