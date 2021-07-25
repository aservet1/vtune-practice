#pragma once

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "DataTypes.hpp"

void print_points(TrainingData& data);

void print_queries(QueryData& data);

void print_training(TrainingData& tdata);

void print_query(QueryData& qdata);

TrainingData read_training(const char* filename);

QueryData read_query(const char* filename);

#define _print(x) std::cout << x << std::endl;

#define PointQueryPrint(pq) std::cout << "( ";					\
							for (int i = 0; (long unsigned int)i < pq.size(); i++)	\
								std::cout << pq[i] << " "; 		\
							std::cout << ")" << std::endl;		\

