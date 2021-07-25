#pragma once
#include "ThreadPool.hpp"
#include "DataTypes.hpp"
#include "tree.hpp"

void
do_parallel_queries(const char* filename, QueryData& qd, TrainingData& td, Node* root, ThreadPool& tp);

// Note that this is not the extra credit, even though the name seems like that. This is just submitting
//   and file-writing a series of queries in parallel, as per the assignment's specifications