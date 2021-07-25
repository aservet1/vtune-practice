#pragma once

#include <queue>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include "myIO.hpp"
#include "tree.hpp"
#include "DataTypes.hpp"
#include "ThreadPool.hpp"

enum treetypes {INTERNAL, LEAF};

struct Node {
	int type;

    static void printall(Node *n); // prints all points in no promised order
	static Node* build_kdtree(TrainingData& td, ThreadPool& threadpool, size_t leaf_capacity);
    static bool verify(Node* root, std::vector<Point>& points);
	static void mydelete(Node* root);
};

struct Internal : Node {
    unsigned int dspliT;
    int value;
    Node *left_child, *right_child;
};

struct Leaf : Node {
	std::vector<Point> points;
};
