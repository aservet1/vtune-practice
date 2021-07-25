#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "tree.hpp"
#include "knn.hpp"
#include "Parallel_K-nn.hpp"
#include "DataTypes.hpp"
#include "myIO.hpp"

Query readQueryFifo(std::string fifo_name){
    std::ifstream queryIn{fifo_name};
    std::string line;
    std::getline(queryIn, line);
	std::stringstream ss( line );

	float coordinate;
	Query query;
	while ( ss >> coordinate ) query.push_back( coordinate );
	return query;
}

void
check_args(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "usage: "
					<< argv[0]
					<< " n_cores training_file query_fifo number_of_neighbors_to_return"
					<< std::endl;
		exit(2);
	}
}

#define console_log(msg) { if (verbose) std::cout << " >> " << msg << std::endl; }

int main(int argc, char *argv[])
{
	auto start_query_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_query_time = std::chrono::system_clock::now() - start_query_time;
	std::vector<Point> neighbors;

	check_args(argc, argv);
	const int n_cores = std::stod(argv[1]);
	const char* training_file = argv[2];
	const char* query_fifo = argv[3];
	const int k = std::stoi(argv[4]);

	bool verbose = true;
	Query q = readQueryFifo(query_fifo);
	PointQueryPrint(q);

	console_log("reading training");
	TrainingData td = read_training(training_file);
	console_log("done");

	console_log("building tree");
	ThreadPool tp(n_cores-1); // cores-1 because main thread also does work in building the tree
	Node* root = Node::build_kdtree(td, tp, 10);
	console_log("done");

	console_log("doing non-parallel query");
	{
		start_query_time = std::chrono::high_resolution_clock::now();
		neighbors = knn(q, k, root);
		elapsed_query_time = std::chrono::system_clock::now() - start_query_time;
		std::cout << "Time to run query: "  << elapsed_query_time.count() << std::endl;
	}
	console_log("done");
	
	if (verbose) std::cout << "neighbors: " << std::endl;
	if (verbose) for (Point p : neighbors) { PointQueryPrint(p); }

	console_log("doing the inefficient knn to check results are valid");
	{
		std::vector<Point> neighbors_2 = inefficient_knn(q, k, td.points);
		if (verbose) std::cout << "neighbors 2.0: " << std::endl;
		if (verbose) for (Point p : neighbors_2) { PointQueryPrint(p); }
		assert(neighbors == neighbors_2);
	}
	console_log("done");

	console_log("doing parallel query");
	{
		start_query_time = std::chrono::high_resolution_clock::now();
		neighbors = parallel_knn(q, k, root);
		elapsed_query_time = std::chrono::system_clock::now() - start_query_time;
		std::cout << "Time to run query: "  << elapsed_query_time.count() << std::endl;
	}
	console_log("done");

	if (verbose) std::cout << "neighbors: " << std::endl;
	if (verbose) for (Point p : neighbors) { PointQueryPrint(p); }

	Node::mydelete(root);
	return 0;
}
