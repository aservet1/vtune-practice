#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>
#include <ctime>

#include "DataTypes.hpp"
#include "myIO.hpp"
#include "tree.hpp"
#include "ThreadPool.hpp"
#include "knn.hpp"
#include "parallel_queries.hpp"

void
check_args(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "usage: "
					<< argv[0]
					<< " n_cores training_file query_file result_file"
					<< std::endl;
		exit(2);
	}
}

bool
does_file_exist(const char* filename) {
	std::ifstream file(filename);
    return (( (file.is_open()) ));
}

#define print_the_time	\
	time_t timetoday;	\
	time (&timetoday);	\
	std::cout << " >>   " << asctime(localtime(&timetoday));

int
main(int argc, char *argv[]) {
	check_args(argc, argv);
	const int n_cores = std::stod(argv[1]);
	const char* training_file = argv[2];
	const char* query_file = argv[3];
	const char* result_file = argv[4];
	if ( does_file_exist(result_file) == true ) {
		std::cerr << "File " << result_file
		<< " already exists. Pick another filename."
		<< std::endl; ;
		   exit(2); ;
	} 
	bool verbose = false;

	/// Read data
	if (verbose) std::cout << " >> reading training" << std::endl;
	TrainingData td;  td = read_training(training_file);
	if (verbose) std::cout << " >> reading query" << std::endl;
	   QueryData qd;  qd = read_query(query_file);

	bool display_points = td.num_points <= 20 && td.num_dimensions <= 10;

	if (verbose) if (display_points) print_training(td);

	/// Build tree
    if (verbose) std::cout << " >> building tree" << std::endl;			//if (verbose) {print_the_time;}
	auto startTreeBuild = std::chrono::high_resolution_clock::now();

	ThreadPool threadpool(n_cores-1); // num threads to have in thread pool is cores-1 to account for main thread
	Node* root = Node::build_kdtree(td, threadpool, 10);

    std::chrono::duration<double> elapsedTreeBuild = std::chrono::system_clock::now() - startTreeBuild;
	std::cout << "Time to build tree: "  << elapsedTreeBuild.count() << std::endl;

	// std::cout << " >> verifying tree" << std::endl;
	// Node::verify(root,td.points);
	// std::cout << " >> verified" << std::endl;
	
	if (verbose) if (display_points) {
		printf("tree:\n");
		Node::printall(root);
	}

	if (verbose) std::cout << " >> running queries, writing to outfile.." << std::endl;
	auto startQuerying = std::chrono::high_resolution_clock::now();

	ThreadPool threadpool2(n_cores-1); // num threads to have in thread pool is cores-1 to account for main thread
	do_parallel_queries(result_file, qd, td, root, threadpool2);

	std::chrono::duration<double> elapsedQuerying = std::chrono::system_clock::now() - startQuerying;
	std::cout << "Time to run queries: "  << elapsedQuerying.count() << std::endl;

	if (verbose) std::cout << " >> deleting tree.." << std::endl;
	Node::mydelete(root);
	root = nullptr;

	if (verbose) std::cout << " >> done" << std::endl;

	return 0;
}
