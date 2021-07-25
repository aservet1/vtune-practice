#include <iostream>
#include "ThreadPool.hpp"
#include "parallel_queries.hpp"
#include "DataTypes.hpp"
#include "errors.hpp"
#include "knn.hpp"

static uint64_t result_id() {
	int fd = open("/dev/urandom", O_RDONLY);
	uint64_t r;
	read(fd,&r,sizeof(r));
	return r;
}

static size_t
write_header(int* fd, TrainingData& td, QueryData& qd) {
	size_t b = 0;
	const char* filetype = "RESULT\0\0";
	b += write(*fd,filetype,8);
	std::vector<uint64_t> header_parameters{
		td.file_id,
		qd.file_id,
		result_id(),
		qd.num_queries,
		qd.num_dimensions,
		qd.num_neighbors_to_return
	};
	for (uint64_t val : header_parameters) {
		b += write(*fd,&val,sizeof(val));
	}
	return b;
}

struct pthreadArgs {
	std::vector<Query> queries;
	int start;
	int end; // non-inclusive
	Node* tree;
	int fd;
	unsigned int k;
	unsigned int num_dimensions;
};

pthread_mutex_t tmplock = PTHREAD_MUTEX_INITIALIZER; // this is just used for me to isolate sections and debug, not part of my actual code
static size_t HEADER_SIZE; // set in do_parallel_queries

void* pthread_knn_writeresult(void* _arg) {
	pthreadArgs args = *((pthreadArgs*)_arg);
	int bytes_per_result = args.k*args.num_dimensions*sizeof(float);
	int bytes_per_point = args.num_dimensions*sizeof(float);
	off_t res_offset = args.start*bytes_per_result;
	for(int XXX = args.start; XXX < args.end; XXX++) {				// for some reason I cannot for the life of me gather, using i as the index was causing weird bugs, so i changed it out for XXX, sorry if it looks bad but there's no way im gonna change it to something else and potentially break it lol :)
		off_t point_offset = 0;
		std::vector<Point> res = knn(args.queries[XXX], args.k, args.tree);
		for(Point p : res) {
			off_t coordinate_offset = 0;
			for ( float coordinate : p ) {
				off_t offset = HEADER_SIZE + res_offset + point_offset + coordinate_offset;
				pwrite(args.fd, &coordinate, sizeof(float), offset);
				coordinate_offset+=sizeof(float);
			}
			point_offset += bytes_per_point;
		}
		res_offset += bytes_per_result;
	}
	return nullptr;
}

void
do_parallel_queries(const char* filename, QueryData& qd, TrainingData& td, Node* root, ThreadPool& threadpool) {

	int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC);
	HEADER_SIZE = write_header(&fd, td, qd);

	int n = threadpool.total_threads();
	std::vector<pthreadArgs> args(n); // stores each arg struct that each pthread routine's 'void* _arg' parameter points to
	std::vector<ThreadToken> thread_tokens(n);

	int startIdx = 0, endIdx;
	for (int i = 0; i < n; i++) {

		endIdx = (i == n - 1)
					? (int)qd.num_queries
					: startIdx + (qd.num_queries/n) ;

		args[i].queries = qd.queries;
		args[i].start = startIdx;
		args[i].end = endIdx;
		args[i].tree = root;
		args[i].fd = fd;
		args[i].k = qd.num_neighbors_to_return;
		args[i].num_dimensions = qd.num_dimensions;

		thread_tokens[i] = threadpool.launchJob(pthread_knn_writeresult ,&args[i]);

		startIdx = endIdx;
	}
	for (int i = 0; i < n; i++) threadpool.join(thread_tokens[i]);

	close(fd);
	chmod(filename, 0666);
}
