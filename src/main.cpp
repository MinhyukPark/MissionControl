#include "mission_control.h"

bool merge_if_possible(MissionControl* mc, std::vector<std::string>* tree_file_name_vec, std::string parent_alignment, std::string left_alignment, std::string right_alignment, std::string alignment_file_prefix) {
    bool merged = true;
    // RAxML-ng to build a tree on each subset
    // reading decomposer output to get the number of subsets
    std::string raxmlng_binary = "raxmlng";
    // and then running raxmlng on each subset if it exists
    std::string predefined_seed = "10101";
    std::string left_sequence_file = left_alignment + ".out";
    std::string right_sequence_file = right_alignment + ".out";
    std::vector<std::string> left_raxmlng_args = {"--msa", left_sequence_file, "--model", "GTR+G" "--seed", predefined_seed, "--threads", RAXML_NG_THREADS, "--prefix", left_alignment, "--all"};
    std::vector<std::string> right_raxmlng_args = {"--msa", right_sequence_file, "--model", "GTR+G" "--seed", predefined_seed, "--threads", RAXML_NG_THREADS, "--prefix", right_alignment, "--all"};
    std::string left_raxmlng_executable_name = "raxmlng" + alignment_file_prefix + "L";
    std::string right_raxmlng_executable_name = "raxmlng" + alignment_file_prefix + "R";
    mc->add_executable(left_raxmlng_executable_name, raxmlng_binary);
    mc->add_executable(right_raxmlng_executable_name, raxmlng_binary);
    mc->update_executable_argument(left_raxmlng_executable_name, left_raxmlng_args);
    mc->update_executable_argument(right_raxmlng_executable_name, right_raxmlng_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(left_raxmlng_executable_name);
    mc->remove_executable(right_raxmlng_executable_name);

    std::string left_model_string;
    std::string right_model_string;
    std::ifstream left_model_output_file(left_alignment + ".ramxl.bestModel");
    std::ifstream right_model_output_file(right_alignment + ".raxml.bestModel");
    left_model_output_file >> left_model_string;
    right_model_output_file >> right_model_string;
    left_model_output_file.close();
    right_model_output_file.close();
    left_model_string = left_model_string.substr(0, left_model_string.find(","));
    right_model_string = right_model_string.substr(0, right_model_string.find(","));
    // build tree on each other's models
    std::vector<std::string> left_cross_raxmlng_args = {"--msa", left_sequence_file, "--seed", predefined_seed, "--threads", RAXML_NG_THREADS,
        "--prefix", left_alignment + "_cross_R",
        "--model", right_model_string, "--all"};
    std::vector<std::string> right_cross_raxmlng_args = {"--msa", right_sequence_file, "--seed", predefined_seed, "--threads", RAXML_NG_THREADS,
        "--prefix", right_alignment + "_cross_L",
        "--model", left_model_string, "--all"};

    std::string left_cross_raxmlng_executable_name = "raxmlng" + alignment_file_prefix + "L_cross_R";
    std::string right_cross_raxmlng_executable_name = "raxmlng" + alignment_file_prefix + "R_cross_L";

    mc->add_executable(left_cross_raxmlng_executable_name, raxmlng_binary);
    mc->add_executable(right_cross_raxmlng_executable_name, raxmlng_binary);
    mc->update_executable_argument(left_cross_raxmlng_executable_name, left_cross_raxmlng_args);
    mc->update_executable_argument(right_cross_raxmlng_executable_name, right_cross_raxmlng_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(left_cross_raxmlng_executable_name);
    mc->remove_executable(right_cross_raxmlng_executable_name);

    std::vector<bool> compatibility_vector = {false, false};


    // collapse and check compatibility
    // collapse using remove edges python
    std::string enter_virtualenv_script = QUICK_SCRIPTS + "enter_virtualenv.sh";
    std::string collapse_virtualenv_location = "/opt/binning/env/bin/activate";
    std::string python2_binary = "python2";
    std::vector<std::string> left_collapse_args = {collapse_virtualenv_location, python2_binary, "/opt/binning/remove_edges_from_tree.py", left_alignment + ".raxml.support",
        SUPPORT_THRESHOLD, left_alignment + "-collapsed.tree", "-strip-both"};
    std::vector<std::string> left_cross_R_collapse_args = {collapse_virtualenv_location, python2_binary, "/opt/binning/remove_edges_from_tree.py", left_alignment + "_cross_R.raxml.support",
        SUPPORT_THRESHOLD, left_alignment + "_cross_R-collapsed.tree", "-strip-both"};
    std::vector<std::string> right_collapse_args = {collapse_virtualenv_location, python2_binary, "/opt/binning/remove_edges_from_tree.py", right_alignment + ".raxml.support",
        SUPPORT_THRESHOLD, right_alignment + "-collapsed.tree", "-strip-both"};
    std::vector<std::string> right_cross_L_collapse_args = {collapse_virtualenv_location, python2_binary, "/opt/binning/remove_edges_from_tree.py", right_alignment + "_cross_L.raxml.support",
        SUPPORT_THRESHOLD, right_alignment + "_cross_L-collapsed.tree", "-strip-both"};

    std::string left_collapse_executable_name = "collapse" + alignment_file_prefix + "L";
    std::string left_cross_R_collapse_executable_name = "collapse" + alignment_file_prefix + "L_cross_R";
    std::string right_collapse_executable_name = "collapse" + alignment_file_prefix + "R";
    std::string right_cross_L_collapse_executable_name = "collapse" + alignment_file_prefix + "R_cross_L";
    mc->add_executable(left_collapse_executable_name, enter_virtualenv_script);
    mc->add_executable(left_cross_R_collapse_executable_name, enter_virtualenv_script);
    mc->add_executable(right_collapse_executable_name, enter_virtualenv_script);
    mc->add_executable(right_cross_L_collapse_executable_name, enter_virtualenv_script);
    mc->update_executable_argument(left_collapse_executable_name, left_collapse_args);
    mc->update_executable_argument(left_cross_R_collapse_executable_name, left_cross_R_collapse_args);
    mc->update_executable_argument(right_collapse_executable_name, right_collapse_args);
    mc->update_executable_argument(right_cross_L_collapse_executable_name, right_cross_L_collapse_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(left_collapse_executable_name);
    mc->remove_executable(left_cross_R_collapse_executable_name);
    mc->remove_executable(right_collapse_executable_name);
    mc->remove_executable(right_cross_L_collapse_executable_name);
    // check compat java
    std::string java_binary = "java";
    std::vector<std::string> left_compat_args = {"-jar", "/opt/binning/phylonet_modified_compat.jar", "compat",
        left_alignment + "-collapsed.tree", left_alignment + "_cross_R-collapsed.tree", "b"};
    std::vector<std::string> right_compat_args = {"-jar", "/opt/binning/phylonet_modified_compat.jar", "compat",
        right_alignment + "-collapsed.tree", right_alignment + "_cross_L-collapsed.tree", "b"};
    std::string left_compat_executable_name = "compat" + alignment_file_prefix + "L";
    std::string right_compat_executable_name = "compat" + alignment_file_prefix + "R";
    mc->add_executable(left_compat_executable_name, java_binary);
    mc->add_executable(right_compat_executable_name, java_binary);
    mc->update_executable_argument(left_compat_executable_name, left_compat_args);
    mc->update_executable_argument(right_compat_executable_name, right_compat_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(left_compat_executable_name);
    mc->remove_executable(right_compat_executable_name);

    std::string left_compat_output;
    std::string right_compat_output;
    std::ifstream left_compat_output_file(mc->format_output_file(left_compat_executable_name));
    std::ifstream right_compat_output_file(mc->format_output_file(right_compat_executable_name));
    std::getline(left_compat_output_file, left_compat_output);
    std::getline(right_compat_output_file, right_compat_output);

    /*
     * False, False = do not merge the subsets
     * True, False = use model B on both subsets after merging
     * False, True = use model A on both subsets after merging
     * True, True = get the average support on bracnhes and pick the larger one's model
     */
    bool left_compat = left_compat_output.compare("0 0 ") == 0;
    bool right_compat = right_compat_output.compare("0 0 ") == 0;
    mc->logger->log_verbose("Compatibility output for " + alignment_file_prefix + " L: " + left_compat_output);
    mc->logger->log_verbose("Compatibility output for " + alignment_file_prefix + " R: " + right_compat_output);
    if(left_compat || right_compat) {
        merged = true;
        std::string merged_tree_file = OUTPUT_DIR + alignment_file_prefix + "merged.tree";
        std::string merge_raxmlng_executable_name = "raxmlng-merge" + alignment_file_prefix;
        mc->add_executable(merge_raxmlng_executable_name, raxmlng_binary);
        if(left_compat && right_compat) {
            std::vector<std::string> merged_raxmlng_args = {"--msa", parent_alignment, "--model", "GTR+G" "--seed", predefined_seed, "--threads", RAXML_NG_THREADS, "--prefix", merge_tree_file, "--all"};
            mc->update_executable_argument(merge_raxmlng_executable_name, merged_raxmlng_args);
        } else if(left_compat) {
            std::vector<std::string> merged_raxmlng_args = {"--msa", parent_alignment, "--seed", predefined_seed, "--threads", RAXML_NG_THREADS,
                "--prefix", merged_tree_file,
                "--model", right_model_string, "--all"};
            mc->update_executable_argument(merge_raxmlng_executable_name, merged_raxmlng_args);
        } else {
            std::vector<std::string> merged_raxmlng_args = {"--msa", parent_alignment, "--seed", predefined_seed, "--threads", RAXML_NG_THREADS,
                "--prefix", merged_tree_file,
                "--model", left_model_string, "--all"};
            mc->update_executable_argument(merge_raxmlng_executable_name, merged_raxmlng_args);
        }
        mc->list_executables();
        mc->run_executables();
        mc->remove_executable(merge_raxmlng_executable_name);
        tree_file_name_vec->push_back(merged_tree_file);
    } else {
        merged = false;
    }
    return merged;
}

// initially called with fasttree.out for tree_file, "rose.aln.true.fasta" for alignment_file,  and SEQUENCE_PARTITION_PREFIX + "" for alignment_file_prefix
int split_and_merge_if_possible_helper(MissionControl* mc, std::vector<std::string>* tree_file_name_vec, std::string tree_file, std::string alignment_file, std::string alignment_file_prefix) {
    // Custom Python Script that uses Vlad's work to decompose the starting tree
    mc->logger->log_verbose("Decomposing " + alignment_file + " with prefix " + alignment_file_prefix);
    std::string python3_binary = "python3";
    std::vector<std::string> decompose_args = {QUICK_SCRIPTS + "split_in_two.py", "--input-tree", tree_file,
                                               "--sequence-file", alignment_file, "--output-prefix", OUTPUT_DIR + alignment_file_prefix,
                                               "--minimum-size", "40"};
    std::string decompose_executable_name = "decompose" + alignment_file_prefix;
    mc->add_executable(decompose_executable_name, python3_binary);
    mc->update_executable_argument(decompose_executable_name, decompose_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(decompose_executable_name);
    int cluster_num = -42;
    std::ifstream decompose_output(mc->format_output_file(decompose_executable_name));
    decompose_output >> cluster_num;
    if(cluster_num == -1) {
        tree_file_name_vec->push_back(tree_file);
        return true;
    }

    std::string left_alignment = OUTPUT_DIR + alignment_file_prefix + "L"; // .out
    std::string right_alignment = OUTPUT_DIR + alignment_file_prefix + "R"; // .out
    bool merged = merge_if_possible(mc, tree_file_name_vec, alignment_file, left_alignment, right_alignment, alignment_file_prefix);
    mc->logger->log_verbose("merged status: ");
    if(merged) {
        mc->logger->log_verbose("true at " + alignment_file_prefix);
    } else {
        mc->logger->log_verbose("false at " + alignment_file_prefix);
    }
    if(merged) {
        return 0;
    } else {
        split_and_merge_if_possible_helper(mc, tree_file_name_vec, left_alignment + ".raxml.bestTree", left_alignment + ".out", alignment_file_prefix + "L");
        split_and_merge_if_possible_helper(mc, tree_file_name_vec, right_alignment + ".raxml.bestTree", right_alignment + ".out", alignment_file_prefix + "R");
    }
    return 1;
}

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

    std::vector<std::string> tree_file_name_vec = {};
    split_and_merge_if_possible_helper(mc, &tree_file_name_vec, starting_tree, starting_alignment, SEQUENCE_PARTITION_PREFIX);
    mc->logger->log_verbose("Final Tree List:");
    for(auto& tree : tree_file_name_vec) {
        mc->logger->log_verbose(tree);
    }


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
    std::string enter_virtualenv_script = QUICK_SCRIPTS + "enter_virtualenv.sh";
    std::string treemerge_virtualenv_location = "/opt/treemerge/env/bin/activate";
    std::string python2_binary = "python";
    std::vector<std::string> treemerge_args = {treemerge_virtualenv_location, python2_binary, "/opt/treemerge/python/treemerge.py", "-s", starting_tree, "-t"};
    for(auto& tree : tree_file_name_vec) {
        treemerge_args.push_back(tree);
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
    mc->add_executable(treemerge_executable_name, enter_virtualenv_script);
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
