#ifndef CONSTS_H
#define CONSTS_H

/* meta */
const std::string CURRENT_RUN_DESCRIPTION = "This is a divide and conquer pipeline with true alignment passed in as input. The FastTree or IQTree starting tree is constructed and decomposed using centroid subset size decomposition. IQTree with model finder is used to build a tree on each subset. Finally TreeMerge is used to merge all the subset trees.";

/* run parameters */
const std::string METHOD_NAME = "NaivePipeline";
const int MAX_PARALLELISM = 120; // fork limit = pthread limit = min(MAX_PARALLELISM, PARALLELISM)
const int PARALLELISM = 10;
const int LOG_LEVEL = 2; // refer to includes/logger.h for the available levels

/* input file parameters */
const std::string INPUT_FILE_NAME = "input_file_name"; // e.g. "true.fasta", "unaligned.fasta"
const std::string CURRENT_DATASET = "1000S1"; // e.g. "1000S1", "1000M1"
const std::string CURRENT_REPLICATE = "R0"; // e.g. "R0", "R1"
const std::string MAX_SUBSET_SIZE_STR = "subset str"; // e.g. "120", "500"
const bool IS_ALIGNED = true; // e.g. true, false
const std::string STARTING_TREE_METHOD = "FastTree"; // e.g. "FastTree", "IQTree"
const std::string SEQUENCE_PARTITION_PREFIX = "sequence_partition_"; // for python decomposer

/* directories */
// system default directories
const std::string USER_SCRATCH = "/scratch/users/minhyuk2/";
const std::string QUICK_SCRIPTS = "/home/minhyuk2/git_repos/QuickScripts/";
const std::string CLUSTER_RUNS_DIR = USER_SCRATCH + "cluster_runs/";
// directories that will be created upon init
const std::string CURRENT_RUN = CURRENT_DATASET + "/" + METHOD_NAME + "/" + MAX_SUBSET_SIZE_STR + "/" + STARTING_TREE_METHOD + "/" + CURRENT_REPLICATE + "/";
const std::string INPUT_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "input/";
const std::string OUTPUT_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "output/";
const std::string ERROR_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "errors/";
const std::string TEMP_WORK_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "tmp/";

#endif
