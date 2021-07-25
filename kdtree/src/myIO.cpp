#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "DataTypes.hpp"
#include "myIO.hpp"

void
print_points(TrainingData& data) {
	// while (data.more_points()) {
	// 	Point point = data.next_point();
	// 	std::cout << "\t( ";
	// 	for (int i = 0; i < data.num_dimensions; i++) {
	// 		std::cout << point[i] << " ";
	// 	}
	// 	std::cout << " )" << std::endl;
	// }
	for (Point p : data.points) {
		PointQueryPrint(p);
	}
}

void
print_queries(QueryData& data) {
	for (Query q : data.queries) {
		PointQueryPrint(q);
	}
	// while (data.more_queries()) {
	// 	Point point = data.next_query();
	// 	std::cout << "\t( ";
	// 	for (int i = 0; i < data.num_dimensions; i++) {
	// 		std::cout << point[i] << " ";
	// 	}
	// 	std::cout << " )" << std::endl;
	// }
}

void
print_training(TrainingData& tdata) {
	std::cout << "file_type: "		<< tdata.file_type		<< std::endl;
	std::cout << "file_id: "		<< tdata.file_id		<< std::endl;
	std::cout << "num_points; "		<< tdata.num_points		<< std::endl;
	std::cout << "num_dimensions: "	<< tdata.num_dimensions	<< std::endl;
	std::cout << "points: "			<< std::endl;
	print_points(tdata);
}

void
print_query(QueryData& qdata) {
	std::cout << "file_type: " << qdata.file_type << std::endl;
	std::cout << "file_id: " << qdata.file_id << std::endl;
	std::cout << "num_queries: " << qdata.num_queries << std::endl;
	std::cout << "num_dimensions: " << qdata.num_dimensions << std::endl;
	std::cout << "num_neighbors_to_return: " << qdata.num_neighbors_to_return << std::endl;
	print_queries(qdata);
}

TrainingData
read_training(const char* filename) {
	TrainingData td;
	int fd = open(filename,O_RDONLY);

	char file_type[9]; file_type[8] = '\0';
	read(fd,file_type,8);
	td.file_type = file_type;

	read(fd, &td.file_id, 8);
	read(fd, &td.num_points, 8);
	read(fd, &td.num_dimensions, 8);

	float coordinate;
	for (long unsigned int _ = 0; _ < (td.num_points); _++) {
		Point p(td.num_dimensions);
		for (long unsigned int i = 0; i < td.num_dimensions; i++) {
			read(fd, &coordinate, 4);
			p[i] = coordinate;
		}
		td.points.push_back(p);	
	}

	close(fd);
	return td;
}

QueryData
read_query(const char* filename) {
	QueryData qdata;
	int fd = open(filename,O_RDONLY);

	char file_type[8];
	read(fd,file_type,8);
	qdata.file_type = file_type;

	read(fd, &qdata.file_id, 8);
	read(fd, &qdata.num_queries, 8);
	read(fd, &qdata.num_dimensions, 8);
	read(fd, &qdata.num_neighbors_to_return, 8);

	Query query(qdata.num_dimensions);
	float coordinate;
	for (uint64_t _ = 0; _ < qdata.num_queries; _++) {
		for (uint64_t i = 0; i < qdata.num_dimensions; i++){
			read(fd, &coordinate, 4);
			query[i] = coordinate;
		}
		qdata.queries.push_back(query);	
	}

	close(fd);
	return qdata;
}
