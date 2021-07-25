#pragma once
#include <iostream>
#include <queue>
#include "ThreadPool.hpp"
#include "DataTypes.hpp"
#include "tree.hpp"

std::vector<Point> parallel_knn(Query query, int k, Node* root);