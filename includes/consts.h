#ifndef CONSTS_H
#define CONSTS_H

/* meta */
const std::string CURRENT_RUN_DESCRIPTION = "This creates constraint trees.";

/* directories */
// system default directories
/* const std::string PROJECTS = "/projects/sciteam/bbaz/minhyuk2/"; // e.g. "/projects/tallis/minhyuk2/", "projects/sciteam/bbaz/minhyuk2" */
/* const std::string QUICK_SCRIPTS = "/u/sciteam/minhyuk2/git_repos/QuickScripts/"; // e.g. "/home/minhyuk2/git_repos/QuickScripts/", "/u/sciteam/minhyuk2/git_repos/QuickScrits/" */
const std::string PROJECTS = "/projects/tallis/minhyuk2/"; // e.g. "/projects/tallis/minhyuk2/", "projects/sciteam/bbaz/minhyuk2"
const std::string SCRATCH = "/home/minhyuk2/scratch/";
const std::string QUICK_SCRIPTS = "/opt/QuickScripts/"; // e.g. "/home/minhyuk2/git_repos/QuickScripts/", "/u/sciteam/minhyuk2/git_repos/QuickScrits/"

/* run parameters */
const std::string METHOD = "CreateConstraintTrees";
const int MAX_PARALLELISM = 12; // fork limit = pthread limit = min(MAX_PARALLELISM, PARALLELISM)
const std::string STARTING_TREE = "IQTree2";
const std::string SUBSET_SIZE = "500";
const std::string TREE_INFERENCE_METHOD = "IQTree2"; // e.g. "FastTree", "IQTree2"
const std::string GUIDE_TREE = "IQTree2";
const std::string IQTREE_THREADS = "4";
const std::string INCOMPLETE_SAMPLING = "0";
const std::string SUPPORT_THRESHOLD = "0";
const std::string RUN_FLAGS = "None"; // e.g. "Support Decompose"
const bool MONITOR_TIME_MEMORY = true; // e.g. true, false
const int PARALLELISM = 3;
const int LOG_LEVEL = 2; // refer to includes/logger.h for the available levels

/* input file parameters */
const std::string INPUT_FILE_NAME = PROJECTS + "input/PASTA/RNASim/1000/1/model/true.fasta"; // e.g. "true.fasta", "unaligned.fasta"
const std::string CURRENT_DATASET = "RNASim1000"; // e.g. "1000S1", "1000M1", "rnasim1000"
const std::string CURRENT_REPLICATE = "1"; // e.g. "R0", "R1"
const bool IS_ALIGNED = true; // e.g. true, false
const std::string SEQUENCE_PARTITION_PREFIX = "sequence_partition_"; // for python decomposer

/* auto created and managed */
// directories that will be created upon init
const std::string CLUSTER_RUNS_DIR = PROJECTS + "cluster_runs/";
const std::string CURRENT_RUN = CURRENT_DATASET + "/" + METHOD + "/" + SUBSET_SIZE + "/" + STARTING_TREE + "/" + CURRENT_REPLICATE + "/";
const std::string INPUT_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "input/";
const std::string OUTPUT_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "output/";
const std::string ERROR_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "errors/";
const std::string TEMP_WORK_DIR = CLUSTER_RUNS_DIR + CURRENT_RUN + "tmp/";

#endif
