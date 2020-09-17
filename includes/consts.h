#ifndef CONSTS_H
#define CONSTS_H

/* meta */
const std::string CURRENT_RUN_DESCRIPTION = "This is a divide and conquer pipeline with true alignment passed in as input. The FastTree or IQTree starting tree is constructed and decomposed using centroid subset size decomposition. IQTree with model finder is used to build a tree on each subset. IQTree is then run on each other's model in a pairwise manner. They are merged based on an algorithim similar to statistical binning. TreeMerge is used to merge all the subset trees.";

/* directories */
// system default directories
const std::string USER_SCRATCH = "/scratch/users/minhyuk2/";
const std::string QUICK_SCRIPTS = "/home/minhyuk2/git_repos/QuickScripts/";
const std::string CLUSTER_RUNS_DIR = USER_SCRATCH + "cluster_runs/";

/* run parameters */
const std::string METHOD_NAME = "PipelineBinning";
const int MAX_PARALLELISM = 120; // fork limit = pthread limit = min(MAX_PARALLELISM, PARALLELISM)
const int PARALLELISM = 10;
const int LOG_LEVEL = 2; // refer to includes/logger.h for the available levels
const std::string BOOTSTRAP_NUM = "1000"; // refer to includes/logger.h for the available levels

/* input file parameters */
const std::string INPUT_FILE_NAME = USER_SCRATCH + "input/vlad_rnasim/1000/R0/true_align.txt"; // e.g. "true.fasta", "unaligned.fasta"
const std::string CURRENT_DATASET = "rnasim1000"; // e.g. "1000S1", "1000M1", "rnasim1000"
const std::string CURRENT_REPLICATE = "R0"; // e.g. "R0", "R1"
const bool IS_ALIGNED = true; // e.g. true, false
const std::string STARTING_TREE_METHOD = "FastTree"; // e.g. "FastTree", "IQTree"
const std::string SEQUENCE_PARTITION_PREFIX = "sequence_partition_"; // for python decomposer

/* auto created and managed */
// directories that will be created upon init
const std::string CURRENT_RUN = CURRENT_DATASET + "/" + METHOD_NAME + "/" + STARTING_TREE_METHOD + "/" + CURRENT_REPLICATE + "/";
const std::string INPUT_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "input/";
const std::string OUTPUT_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "output/";
const std::string ERROR_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "errors/";
const std::string TEMP_WORK_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "tmp/";

#endif
