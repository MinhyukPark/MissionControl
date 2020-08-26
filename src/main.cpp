#include "mission_control.h"

int main(int argc, char* argv[]) {
    Logger* logger = new Logger(LogLevel::VERBOSE);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t c_now = std::chrono::system_clock::to_time_t(now);
    struct tm* tm_struct;
    char tm_buffer[80];
    tm_struct = localtime(&c_now);
    strftime(tm_buffer, sizeof(tm_buffer), "%Y-%m-%d_%H-%M-%S", tm_struct);
    std::string current_datetime(tm_buffer);
    std::string current_run = "MissionControl-Pipeline-" + current_datetime;
    MissionControl* mc = new MissionControl();
    int PARALLELISM = 4;
    mc->init_mission_control(current_run, PARALLELISM, logger);
    mc->test_mission_control();
    mc->logger->log_info("Started Main");

    std::string input_alignment = INPUT_DIR + "rose.aln.true.fasta";

    if(false) {
        // [optional] MAFFT to build the MSA
        std::string mafft_binary = "mafft";
        std::vector<std::string> mafft_args = {input_alignment};
        std::string mafft_executable_name = "mafft";
        mc->add_executable(mafft_executable_name, mafft_binary);
        mc->update_executable_argument(mafft_executable_name, mafft_args);
        mc->list_executables();
        mc->run_executables();
        mc->remove_executable(mafft_executable_name);
        input_alignment = mc->format_output_file(mafft_executable_name);
    }

    // FastTree to build the starting tree from MSA
    std::string fasttree_binary = "fasttree";
    std::vector<std::string> fasttree_args = {input_alignment};
    std::string fasttree_executable_name = "fasttree";
    mc->add_executable(fasttree_executable_name, fasttree_binary);
    mc->update_executable_argument(fasttree_executable_name, fasttree_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(fasttree_executable_name);
    // Custom Python Script that uses Vlad's work to decompose the starting tree
    std::string python3_binary = "python3";
    std::string max_subset_size_str = "10";
    std::string sequence_partition_prefix = "sequence_partition_";
    std::vector<std::string> decompose_args = {QUICK_SCRIPTS + "decompose.py", mc->format_output_file(fasttree_executable_name), max_subset_size_str,
                                               input_alignment, OUTPUT_DIR + mc->current_run + sequence_partition_prefix};
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
        std::string current_sequence_file = OUTPUT_DIR + mc->current_run + "sequence_partition_" + std::to_string(i) + ".out";
        std::ifstream current_sequence_partition_file(current_sequence_file);
        if (current_sequence_partition_file.is_open()) {
            current_sequence_partition_file.close();
            std::vector<std::string> iqtree_args = {"-s", current_sequence_file, "-m", "MFP", "-seed", predefined_seed};
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
    // TreeMerge to merge the trees
    // using astrid first to get the AGID matrix and taxlist
    std::string astrid_binary = "astrid";
    std::string astrid_matrix_filename = OUTPUT_DIR + mc->current_run + "astrid.mat";
    std::string astrid_matrix_taxlist_filename = OUTPUT_DIR + mc->current_run + "astrid.mat_taxlist";
    std::vector<std::string> astrid_args = {"-i", mc->format_output_file(fasttree_executable_name), "-c", astrid_matrix_filename};
    std::string astrid_executable_name = "astrid";
    mc->add_executable(astrid_executable_name, astrid_binary);
    mc->update_executable_argument(astrid_executable_name, astrid_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(astrid_executable_name);
    // and then using treemerge on the final set of inputs
    std::string python2_binary = "python";
    /* std::vector<std::string> treemerge_args = {"/opt/treemerge/python/treemerge.py", "-s", mc->format_output_file(fasttree_executable_name), "-t"}; */
    std::vector<std::string> treemerge_args = {"/home/minhyuk2/git_repos/treemerge/python/treemerge.py", "-s", mc->format_output_file(fasttree_executable_name), "-t"};
    for(int i = 0; i < cluster_size; i ++) {
        std::string current_sequence_treefile = OUTPUT_DIR + mc->current_run + "sequence_partition_" + std::to_string(i) + ".out.treefile";
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
    treemerge_args.push_back(OUTPUT_DIR + mc->current_run + "treemerge.tree");
    treemerge_args.push_back("-w");
    treemerge_args.push_back("/tmp/" + current_run); // not mc->current_run bc trailing slash
    treemerge_args.push_back("-p");
    treemerge_args.push_back("paup");
    std::string treemerge_executable_name = "treemerge";
    mc->add_executable(treemerge_executable_name, python2_binary);
    mc->update_executable_argument(treemerge_executable_name, treemerge_args);
    mc->list_executables();
    mc->run_executables();
    mc->remove_executable(treemerge_executable_name);
    mc->logger->log_info("End experiments");
    delete mc;
}
