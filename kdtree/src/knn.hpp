#pragma once

#include <queue>
#include <assert.h>
#include <vector>
#include <functional>
#include "tree.hpp"
#include "DataTypes.hpp"
#include <bits/stdc++.h>

std::vector<Point>
knn(Point queryPoint, unsigned int k, Node* root);

std::vector<Point>
inefficient_knn(Query query, int k, std::vector<Point>& points);