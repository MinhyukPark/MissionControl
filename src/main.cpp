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
    if(IS_ALIGNED) {
        starting_alignment = INPUT_FILE_NAME;
    }
    // FastTree to build the starting tree from MSA
    std::string starting_tree;
    std::string guide_tree;
    if(STARTING_TREE == "Internal" || GUIDE_TREE == "Internal") {
        std::string fasttree_binary = "fasttreeMP";
        std::vector<std::string> fasttree_internal_args = {"-noml", "-nome", "-nt", "-gtr", "-gamma", starting_alignment};
        std::string fasttree_internal_executable_name = "fasttree-int";
        mc->add_executable(fasttree_internal_executable_name, fasttree_binary);
        mc->update_executable_argument(fasttree_internal_executable_name, fasttree_internal_args);
        mc->list_executables();
        mc->run_executables();
        /* mc->logger->log_info("not running fasttree internal"); */
        mc->remove_executable(fasttree_internal_executable_name);
        if(STARTING_TREE == "Internal") {
            starting_tree = mc->format_output_file(fasttree_internal_executable_name);
        }
        if(GUIDE_TREE == "Internal") {
            guide_tree = mc->format_output_file(fasttree_internal_executable_name);
        }
    }
    if(STARTING_TREE == "FastTree" || GUIDE_TREE == "FastTree") {
        std::string fasttree_binary = "fasttreeMP";
        std::vector<std::string> fasttree_args = {"-nt", "-gtr", "-gamma", starting_alignment};
        std::string fasttree_executable_name = "fasttree";
        mc->add_executable(fasttree_executable_name, fasttree_binary);
        mc->update_executable_argument(fasttree_executable_name, fasttree_args);
        mc->list_executables();
        mc->run_executables();
        /* mc->logger->log_info("not running fasttree full"); */
        mc->remove_executable(fasttree_executable_name);
        if(STARTING_TREE == "FastTree") {
            starting_tree = mc->format_output_file(fasttree_executable_name);
        }
        if(GUIDE_TREE == "FastTree") {
            guide_tree = mc->format_output_file(fasttree_executable_name);
        }
    }
    if(STARTING_TREE == "MAFFT" || GUIDE_TREE == "MAFFT") {
        std::string mafft_binary = "mafft";
        std::vector<std::string> mafft_args = {"--retree", "0", "--treeout", "--parttree", "--quiet", "--thread", "4", "--anysymbol", "--partsize", "1000", starting_alignment};
        std::string mafft_executable_name = "mafft";
        mc->add_executable(mafft_executable_name, mafft_binary);
        mc->update_executable_argument(mafft_executable_name, mafft_args);
        mc->list_executables();
        /* mc->run_executables(); */
        mc->logger->log_info("not running mafft");
        mc->remove_executable(mafft_executable_name);
        if(STARTING_TREE == "MAFFT") {
            starting_tree = OUTPUT_DIR + "mafft.tree";
        }
        if(GUIDE_TREE == "MAFFT") {
            starting_tree = OUTPUT_DIR + "mafft.tree";
        }
    }
    if(STARTING_TREE == "ClustalOmega" || GUIDE_TREE == "ClustalOmega") {
        std::string clustalomega_binary = "clustalo";
        std::vector<std::string> clustalomega_args = {"-i", starting_alignment, "--max-hmm-iterations=-1", "--guidetree-out=clustalomega.tree", "--threads=4"};
        std::string clustalomega_executable_name = "clustalo";
        mc->add_executable(clustalomega_executable_name, clustalomega_binary);
        mc->update_executable_argument(clustalomega_executable_name, clustalomega_args);
        mc->list_executables();
        mc->run_executables();
        mc->remove_executable(clustalomega_executable_name);
        if(STARTING_TREE == "ClustalOmega") {
            starting_tree = OUTPUT_DIR + "clustalomega.tree";
        }
        if(GUIDE_TREE == "ClustalOmega") {
            guide_tree = OUTPUT_DIR + "clustalomega.tree";
        }
    }
    if(STARTING_TREE == "IQTree2" || GUIDE_TREE == "IQTree2") {
        std::string iqtree2_binary = "iqtree2";
        std::string predefined_seed = "10101";
        std::string iqtree2_executable_name = "iqtree2-complete";
        std::vector<std::string> iqtree2_args = {"-s", starting_alignment, "-nt", "AUTO", "-ntmax", IQTREE_THREADS, "-seed", predefined_seed, "-m", "GTR+G", "-pre", OUTPUT_DIR + "iqtree-full"};
        mc->add_executable(iqtree2_executable_name, iqtree2_binary);
        mc->update_executable_argument(iqtree2_executable_name, iqtree2_args);
        mc->list_executables();
        mc->run_executables();
        /* mc->logger->log_info("not running iqtree full"); */
        mc->remove_executable(iqtree2_executable_name);
        if(STARTING_TREE == "IQTree2") {
            starting_tree = OUTPUT_DIR + "iqtree-full.treefile";
        }
        if(GUIDE_TREE == "IQTree2") {
            guide_tree = OUTPUT_DIR + "iqtree-full.treefile";
        }
    }


    std::string enter_virtualenv_script = QUICK_SCRIPTS + "enter_virtualenv.sh";
    std::string quickscripts_virtualenv_location = "/opt/QuickScripts/env/bin/activate";
    std::string python3_binary = "python3";
    std::vector<std::string> decompose_args = {quickscripts_virtualenv_location, python3_binary, QUICK_SCRIPTS + "decompose.py", "--input-tree", starting_tree,
                                               "--sequence-file", starting_alignment, "--output-prefix", OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX,
                                               "--maximum-size", SUBSET_SIZE};
    if(RUN_FLAGS == "Longest") {
        decompose_args.push_back("--longest");
    } else if(RUN_FLAGS == "Heuristic Decompose") {
        decompose_args.push_back("--mode");
        decompose_args.push_back("heuristic");
    }

    if(GUIDE_TREE == "Incomplete") {
        decompose_args.push_back("--incomplete");
        decompose_args.push_back(INCOMPLETE_SAMPLING);
    }
    std::string decompose_executable_name = "decompose";
    mc->add_executable(decompose_executable_name, enter_virtualenv_script);
    mc->update_executable_argument(decompose_executable_name, decompose_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(decompose_executable_name);

    std::vector<std::string> tree_file_name_vec = {};
    // raxml-ng to build a tree on each subset
    // reading decomposer output to get the number of subsets
    if (TREE_INFERENCE_METHOD == "RAxML-ng") {
        std::string raxmlng_binary = "raxmlng";
        std::string predefined_seed = "10101";
        int cluster_size;
        std::ifstream decompose_output_file(mc->format_output_file(decompose_executable_name));
        decompose_output_file >> cluster_size;
        decompose_output_file.close();
        for(int i = 0; i < cluster_size; i ++) {
            std::string current_sequence_file = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i) + ".out";
            std::ifstream current_sequence_partition_file(current_sequence_file);
            if (current_sequence_partition_file.is_open()) {
                current_sequence_partition_file.close();
                std::string raxmlng_executable_name = "raxmlng-" + std::to_string(i);
                std::string current_sequence_alignment_prefix = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i);
                tree_file_name_vec.push_back(current_sequence_alignment_prefix + ".raxml.bestTree");
                std::vector<std::string> raxmlng_args = {"--msa", current_sequence_file, "--model", "GTR+G", "--seed", predefined_seed, "--threads", IQTREE_THREADS, "--prefix", current_sequence_alignment_prefix};
                mc->add_executable(raxmlng_executable_name, raxmlng_binary);
                mc->update_executable_argument(raxmlng_executable_name, raxmlng_args);
            }
        }
        mc->list_executables();
        mc->run_executables();
        for(int i = 0; i < cluster_size; i ++) {
            std::string raxmlng_executable_name = "raxmlng-" + std::to_string(i);
            mc->remove_executable(raxmlng_executable_name);
        }
    } else if (TREE_INFERENCE_METHOD == "IQTree2") {
        std::string iqtree2_binary = "iqtree2";
        std::string predefined_seed = "10101";
        int cluster_size;
        std::ifstream decompose_output_file(mc->format_output_file(decompose_executable_name));
        decompose_output_file >> cluster_size;
        decompose_output_file.close();
        for(int i = 0; i < cluster_size; i ++) {
            std::string current_sequence_file = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i) + ".out";
            std::ifstream current_sequence_partition_file(current_sequence_file);
            if (current_sequence_partition_file.is_open()) {
                current_sequence_partition_file.close();
                std::string iqtree2_executable_name = "iqtree2-" + std::to_string(i);
                std::string current_sequence_alignment_prefix = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i);
                tree_file_name_vec.push_back(current_sequence_alignment_prefix + ".treefile");
                std::vector<std::string> iqtree2_args = {"-s", current_sequence_file, "-nt", "AUTO", "-ntmax", IQTREE_THREADS, "-seed", predefined_seed, "-m", "GTR+G", "-pre", current_sequence_alignment_prefix};
                mc->add_executable(iqtree2_executable_name, iqtree2_binary);
                mc->update_executable_argument(iqtree2_executable_name, iqtree2_args);
            }
        }
        mc->list_executables();
        mc->run_executables();
        for(int i = 0; i < cluster_size; i ++) {
            std::string iqtree2_executable_name = "iqtree2-" + std::to_string(i);
            mc->remove_executable(iqtree2_executable_name);
        }
    } else if (TREE_INFERENCE_METHOD == "FastTree") {
        std::string fasttree_binary = "fasttreeMP";
        int cluster_size;
        std::ifstream decompose_output_file(mc->format_output_file(decompose_executable_name));
        decompose_output_file >> cluster_size;
        decompose_output_file.close();
        for(int i = 0; i < cluster_size; i ++) {
            std::string current_sequence_file = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + std::to_string(i) + ".out";
            std::ifstream current_sequence_partition_file(current_sequence_file);
            if (current_sequence_partition_file.is_open()) {
                current_sequence_partition_file.close();
                std::string fasttree_executable_name = "fasttree-" + std::to_string(i);
                std::vector<std::string> fasttree_args = {"-nt", "-gtr", "-gamma", current_sequence_file};
                mc->add_executable(fasttree_executable_name, fasttree_binary);
                mc->update_executable_argument(fasttree_executable_name, fasttree_args);
                tree_file_name_vec.push_back(mc->format_output_file(fasttree_executable_name));
            }
        }
        mc->list_executables();
        mc->run_executables();
        for(int i = 0; i < cluster_size; i ++) {
            std::string fasttree_executable_name = "fasttree-" + std::to_string(i);
            mc->remove_executable(fasttree_executable_name);
        }
    }

    mc->logger->log_verbose("Final Tree List:");
    for(auto& tree : tree_file_name_vec) {
        mc->logger->log_verbose(tree);
    }


    if(GUIDE_TREE == "Incomplete") {
        std::string incomplete_sequence_file = OUTPUT_DIR + SEQUENCE_PARTITION_PREFIX + "incomplete.out";
        std::string iqtree2_binary = "iqtree2";
        std::string predefined_seed = "10101";
        std::string iqtree2_executable_name = "iqtree2-incomplete";
        std::vector<std::string> iqtree2_args = {"-s", incomplete_sequence_file, "-nt", "AUTO", "-ntmax", IQTREE_THREADS, "-seed", predefined_seed, "-m", "GTR+G", "-pre", OUTPUT_DIR + "guide"};
        mc->add_executable(iqtree2_executable_name, iqtree2_binary);
        mc->update_executable_argument(iqtree2_executable_name, iqtree2_args);
        mc->list_executables();
        mc->run_executables();
        mc->remove_executable(iqtree2_executable_name);
        guide_tree = OUTPUT_DIR + "guide.treefile";
    }

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
