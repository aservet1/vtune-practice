#pragma once

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

typedef std::vector<float> Point;
typedef std::vector<float> Query;

struct TrainingData {
	private:
		int cursor = 0;
	public:
		TrainingData() { cursor = 0; }

		// TrainingData(TrainingData& o) : //file_type{o.file_type}, file_id{o.file_id},
		// 	num_points{o.num_points},
		// 	num_dimensions{o.num_dimensions}//, points{o.points}
		// {}

		std::string file_type;
		uint64_t file_id;
		uint64_t num_points;
		uint64_t num_dimensions;
		std::vector<Point> points;
		// std::vector<float> points;
		// Point next_point() {
		// 	std::vector<float> point(num_dimensions);
		// 	for (int i = 0; i < num_dimensions; i++) {
		// 		point[i] = points[cursor++];
		// 	}
		// 	return point;
		// }
		// bool more_points() {
		// 	return cursor < points.size();
		// }
		// void reset_cursor() {
		// 	cursor = 0;
		// }
		// Point randomPoint(unsigned int* rseed) {
		// 	Point p(num_dimensions);
		// 	int start = (rand_r(rseed) % num_points) * num_dimensions;
		// 	for (int i = 0; i < num_dimensions; i++)
		// 		p[i] = points[start+i];
		// 	return p;
		// }
};

struct QueryData {
	// private:
		// int cursor = 0;
	public:
		std::string file_type;
		uint64_t file_id;
		uint64_t num_queries;
		uint64_t num_dimensions;
		uint64_t num_neighbors_to_return;
		std::vector<Query> queries;
		// std::vector<float> queries;
		// Query next_query() {
		// 	Query query(num_dimensions);
		// 	for (int i = 0; i < num_dimensions; i++) {
		// 		query[i] = queries[cursor++];
		// 	}
		// 	return query;
		// }
		// bool more_queries() {
		// 	return cursor < queries.size();
		// }
		// void reset_cursor() {
		// 	cursor = 0;
		// }
		// Query randomQuery(unsigned int* rseed) {
		// 	Query q(num_dimensions);
		// 	int start = (rand_r(rseed) % num_queries) * num_dimensions;
		// 	for (int i = 0; i < num_dimensions; i++)
		// 		q[i] = queries[start+i];
		// 	return q;
		// }
};

		// void split_points(Point median, int dimension, std::vector<float>& lo, std::vector<float>& hi) {
		// 	assert(dimension < num_dimensions);
		// 	float m = median[dimension];
		// 	bool foundMedianOnce = false;
		// 	for (int i = 0; i < num_points*num_dimensions; i += num_dimensions ) {
		// 		if (points[i+dimension] > m) {
		// 			for (int j = 0; j < num_dimensions; j++)
		// 				hi.push_back(points[i+j]);
		// 		} else if (points[i+dimension] < m) {
		// 			for (int j = 0; j < num_dimensions; j++) {
		// 				lo.push_back(points[i+j]);
		// 			}
		// 		} else if (points[i+dimension] == m) {
		// 			if (std::equal(
		// 				&points[i], &points[i+num_dimensions],
		// 				median.begin(), median.end()))
		// 			{
		// 				if (foundMedianOnce) {
		// 					for (int j = 0; j < num_dimensions; j++) {
		// 						lo.push_back(points[i+j]);
		// 					}
		// 				} else {
		// 					foundMedianOnce = true;
		// 				}
		// 			} else {
		// 				for (int j = 0; j < num_dimensions; j++) {
		// 					lo.push_back(points[i+j]);
		// 				}
		// 			}
		// 		}
		// 	}
		// }

//		std::vector<float>
//		upper_half(Point median, int dimension) {
//			assert(dimension < num_dimensions);
//			std::vector<float> upperhalf;
//			float m = median[dimension];
//			for (int i = 0; i < num_points*num_dimensions; i += num_dimensions ) {
//				if (points[i+dimension] > m)
//					for (int j = 0; j < num_dimensions; j++)
//						upperhalf.push_back(points[i+j]);
//			}
//			return upperhalf;
//		}
//
//		std::vector<float>
//		lower_half(Point median, int dimension) {
//			assert(dimension < num_dimensions);
//			std::vector<float> lowerhalf;
//			float m = median[dimension];
//			bool foundMedianOnce = false;
//			for (int i = 0; i < num_points*num_dimensions; i += num_dimensions ) {
//				if (points[i+dimension] < m) {
//					for (int j = 0; j < num_dimensions; j++) {
//						lowerhalf.push_back(points[i+j]);
//					}
//				} else if (points[i+dimension] == m) {
//					if (std::equal(
//						&points[i], &points[i+num_dimensions],
//						median.begin(), median.end()))
//					{
//						if (foundMedianOnce) {
//							for (int j = 0; j < num_dimensions; j++) {
//								lowerhalf.push_back(points[i+j]);
//							}
//						} else {
//							foundMedianOnce = true;
//						}
//					} else {
//						for (int j = 0; j < num_dimensions; j++) {
//							lowerhalf.push_back(points[i+j]);
//						}
//					}
//				}
//			}
//			return lowerhalf;
//		}