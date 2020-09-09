#include "mission_control.h"

int main(int argc, char* argv[]) {
    std::chrono::system_clock::time_point initial_time = std::chrono::system_clock::now();
    std::time_t c_initial = std::chrono::system_clock::to_time_t(initial_time);
    struct tm* initial_tm_struct;
    char initial_tm_buffer[80];
    initial_tm_struct = localtime(&c_initial);
    strftime(initial_tm_buffer, sizeof(initial_tm_buffer), "%Y-%m-%d_%H-%M-%S", initial_tm_struct);
    std::string initial_datetime(initial_tm_buffer);

    MissionControl* mc = new MissionControl();
    mc->init_mission_control();
    mc->test_mission_control();
    mc->logger->log_info("Started Main at " + initial_datetime);

    std::string starting_alignment;
    if(!IS_ALIGNED) {
        // [optional] MAFFT to build the MSA
        std::string mafft_binary = "mafft";
        std::vector<std::string> mafft_args = {INPUT_FILE_NAME};
        std::string mafft_executable_name = "mafft";
        mc->add_executable(mafft_executable_name, mafft_binary);
        mc->update_executable_argument(mafft_executable_name, mafft_args);
        mc->list_executables();
        mc->run_executables();
        mc->remove_executable(mafft_executable_name);
        starting_alignment = mc->format_output_file(mafft_executable_name);
    } else {
        starting_alignment = INPUT_FILE_NAME;
    }

    // FastTree to build the starting tree from MSA
    std::string starting_tree;
    if(STARTING_TREE_METHOD == "FastTree") {
        std::string fasttree_binary = "fasttree";
        std::vector<std::string> fasttree_args = {"-nt", "-gtr", "-gamma", starting_alignment};
        std::string fasttree_executable_name = "fasttree";
        mc->add_executable(fasttree_executable_name, fasttree_binary);
        mc->update_executable_argument(fasttree_executable_name, fasttree_args);
        mc->list_executables();
        mc->run_executables();
        mc->remove_executable(fasttree_executable_name);
        starting_tree = mc->format_output_file(fasttree_executable_name);
    }

    // Custom Python Script that uses Vlad's work to decompose the starting tree
    std::string python3_binary = "python3";
    std::vector<std::string> decompose_args = {QUICK_SCRIPTS + "decompose.py", starting_tree, MAX_SUBSET_SIZE_STR,
                                               starting_alignment, OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX};
    std::string decompose_executable_name = "decompose";
    mc->add_executable(decompose_executable_name, python3_binary);
    mc->update_executable_argument(decompose_executable_name, decompose_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(decompose_executable_name);

    // IQTree to build a tree on each subset
    // reading decomposer output to get the number of subsets
    std::string iqtree_binary = "iqtree";
    int cluster_size;
    std::ifstream decompose_output_file(mc->format_output_file(decompose_executable_name));
    decompose_output_file >> cluster_size;
    decompose_output_file.close();
    // and then running iqtree on each subset if it exists
    std::string predefined_seed = "10101";
    for(int i = 0; i < cluster_size; i ++) {
        std::string current_sequence_file = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i) + ".out";
        std::ifstream current_sequence_partition_file(current_sequence_file);
        if (current_sequence_partition_file.is_open()) {
            current_sequence_partition_file.close();
            std::vector<std::string> iqtree_args = {"-s", current_sequence_file, "-seed", predefined_seed, "-nt", "AUTO"};
            std::string iqtree_executable_name = "iqtree" + std::to_string(i);
            mc->add_executable(iqtree_executable_name, iqtree_binary);
            mc->update_executable_argument(iqtree_executable_name, iqtree_args);
        }
    }
    mc->list_executables();
    mc->run_executables();
    for(int i = 0; i < cluster_size; i ++) {
        std::string iqtree_executable_name = "iqtree" + std::to_string(i);
        mc->remove_executable(iqtree_executable_name);
    }
    /*
    // for every subset, run with every other model
    //     - create a map of subset to model
    //     - and then run with model for every subset
    std::map<int, std::string> subset_model_map;
    for(int i = 0; i < cluster_size; i ++) {
        std::string current_model_filename = OUTPUT_DIR + mc->CURRENT_RUN + current_decompose_directory + "sequence_partition_" + std::to_string(i) + ".out.iqtree";
        std::ifstream current_model_file(current_sequence_filename);
        if (current_model_file.is_open()) {
            std::string current_model = "";
            // parse the file
            std::string cur_line;
            std::string search_target = "Best-fit model according to BIC: ";
            while(getline(current_model_file, cur_line)) {
                if(cur_line.find(search_target, 0) != string::npos) {

                }
            }
            current_model_file.close();
            subset_model_map[i] = current_model;
        }
    }
    // for every subset tree output, calculate the rf-rate and store in a matrix
    //     - mat[0][2] is tree 0 on tree 2's model, mat[2][0] is tree 2 on tree 0's model
    // while there is still an entry in a matrix that is sufficiently low, merge and run iqtree on the merged subset using one of the models

    bool did_merge = true;
    while(did_merge) {
    }
    */

    // TreeMerge to merge the trees
    // using astrid first to get the AGID matrix and taxlist
    std::string astrid_binary = "astrid";
    std::string astrid_matrix_filename = OUTPUT_DIR + "astrid.mat";
    std::string astrid_matrix_taxlist_filename = OUTPUT_DIR + "astrid.mat_taxlist";
    std::vector<std::string> astrid_args = {"-i", starting_tree, "-c", astrid_matrix_filename};
    std::string astrid_executable_name = "astrid";
    mc->add_executable(astrid_executable_name, astrid_binary);
    mc->update_executable_argument(astrid_executable_name, astrid_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(astrid_executable_name);
    // and then using treemerge on the final set of inputs
    std::string python2_binary = "python";
    std::vector<std::string> treemerge_args = {"/opt/treemerge/python/treemerge.py", "-s", starting_tree, "-t"};
    for(int i = 0; i < cluster_size; i ++) {
        std::string current_sequence_treefile = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i) + ".out.treefile";
        std::ifstream current_sequence_partition_treefile(current_sequence_treefile);
        if (current_sequence_partition_treefile.is_open()) {
            current_sequence_partition_treefile.close();
            treemerge_args.push_back(current_sequence_treefile);
        }
    }
    treemerge_args.push_back("-m");
    treemerge_args.push_back(astrid_matrix_filename);
    treemerge_args.push_back("-x");
    treemerge_args.push_back(astrid_matrix_taxlist_filename);
    treemerge_args.push_back("-o");
    treemerge_args.push_back(OUTPUT_DIR + "treemerge.tree");
    treemerge_args.push_back("-w");
    treemerge_args.push_back(TEMP_WORK_DIR);
    treemerge_args.push_back("-p");
    treemerge_args.push_back("paup");
    std::string treemerge_executable_name = "treemerge";
    mc->add_executable(treemerge_executable_name, python2_binary);
    mc->update_executable_argument(treemerge_executable_name, treemerge_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(treemerge_executable_name);

    std::chrono::system_clock::time_point final_time = std::chrono::system_clock::now();
    std::time_t c_final = std::chrono::system_clock::to_time_t(final_time);
    struct tm* final_tm_struct;
    char final_tm_buffer[80];
    final_tm_struct = localtime(&c_final);
    strftime(final_tm_buffer, sizeof(final_tm_buffer), "%Y-%m-%d_%H-%M-%S", final_tm_struct);
    std::string final_datetime(final_tm_buffer);
    mc->logger->log_info("End experiments at " + final_datetime);
    delete mc;
}
