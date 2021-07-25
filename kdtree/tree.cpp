
#include <queue>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <functional>
#include <algorithm>
#include "myIO.hpp"
#include "DataTypes.hpp"
#include "ThreadPool.hpp"
#include "tree.hpp"

void Node::mydelete(Node* root) { // you have to call this instead of normal 'delete' to clean up memory
	if (root->type == INTERNAL) {
		Internal* i = (Internal*)root;
		mydelete((Node*)(i->left_child));
		mydelete((Node*)(i->right_child));
		delete i;
	}
	else if (root->type == LEAF) {
		delete (Leaf*)root;
	}
	else {
		fprintf(stderr, "Invalid node type: %d !! in delete()\n", root->type);
		exit(107);
	}
}

void aux_verify(Node* root, std::vector<Point>& point_collect) {
	if (root->type == LEAF) {
		for (Point p : ((Leaf*)root)->points )
			point_collect.push_back(p);
		return;
	}
	else {
		Internal* n = (Internal*)root;
		if (n->left_child)  aux_verify(n->left_child,  point_collect);
		if (n->right_child) aux_verify(n->right_child, point_collect);
	}
}

bool Node::verify(Node* root, std::vector<Point>& points) {
	std::vector<Point> collected;
	aux_verify(root, collected);
	auto cmp = [](Point a, Point b) {
		return a[0] < b[0]; // sort by first dimension
	};
	std::sort(points.begin(), points.end(), cmp);
	std::sort(collected.begin(), collected.end(), cmp);
	return(points == collected);
}

void Node::printall(Node* root) {
	if (root->type == LEAF) {
		for (Point p : ((Leaf*)root)->points) {
			PointQueryPrint(p);
		}
	} else {
		Internal* n = (Internal*)root;
		if(n->left_child)printall(n->left_child);
		if(n->right_child)printall(n->right_child);
	}
}

Point median(std::vector<Point>& v, int dimension) {
    auto mycmp = [dimension](Point a, Point b) {
        return a[dimension] < b[dimension];
    };  
    int median_offset = v.size() / 2;
    std::nth_element(v.begin(), v.begin()+median_offset, v.end(), mycmp);
    return v[median_offset];
}

static void
split_points(std::vector<Point>& points,
			Point median,
			int dimension,
			int num_dimensions,
			std::vector<Point>& lo,
			std::vector<Point>& hi)
{
	assert(dimension < num_dimensions);
	float m = median[dimension];
	for (Point p : points ) {
		if (p[dimension] > m) {
			hi.push_back(p);
		} else  {
			lo.push_back(p);
		}
	}
}

struct treebuildarg {
	treebuildarg(
			std::vector<Point>& points,
			unsigned int dimension,
			unsigned int num_dimensions,
			ThreadPool& threadpool,
			size_t leaf_capacity) :
		points{points},
		dimension{dimension},
		num_dimensions{num_dimensions},
		threadpool{threadpool},
		leaf_capacity{leaf_capacity}
	{
		result = nullptr;
	}

	std::vector<Point>& points;
	unsigned int dimension;
	unsigned int num_dimensions;
	ThreadPool& threadpool;
	size_t leaf_capacity;
	Node* result;
};

Node* kdsubtree(std::vector<Point>&, unsigned int, unsigned int, ThreadPool&, size_t leaf_capacity);

void* pthreaded_kdsubtree(void* _arg) {
	treebuildarg* arg = (treebuildarg*)_arg;
	arg->result = kdsubtree( arg->points, arg->dimension, arg->num_dimensions, arg->threadpool, arg->leaf_capacity);
	return NULL;
}

//#define MAX_POINTS_PER_LEAF 10
//static size_t LEAF_CAPACITY;
Node* kdsubtree(std::vector<Point>& points,
				unsigned int dimension,
				unsigned int num_dimensions,
				ThreadPool& threadpool,
				size_t leaf_capacity)
{
	if (points.size() <= leaf_capacity) {
		Leaf* l = new Leaf;
		l->type = LEAF;
		l->points = points;
		return l;
	}

	Point m = median(points, dimension);

	Internal* n = new Internal;
	n->type = INTERNAL;
	n->value = m[dimension];
	n->dspliT = dimension;

	std::vector<Point> lo_pts, hi_pts;

	split_points(points, m, dimension, num_dimensions, lo_pts, hi_pts); //TODO if you get a sec, just yeet this and work off of one array with range pointers instead of copying subsections
	unsigned int nextDimension = (dimension+1)%num_dimensions;
	
	ThreadToken lo_tok = -1, hi_tok = -1;
	treebuildarg lo_arg(lo_pts, dimension, num_dimensions, threadpool, leaf_capacity);
	treebuildarg hi_arg(hi_pts, dimension, num_dimensions, threadpool, leaf_capacity);
	
	lo_tok = threadpool.launchJob(pthreaded_kdsubtree, &lo_arg);
	if (lo_tok == -1) n->left_child  = kdsubtree(lo_pts,nextDimension,num_dimensions,threadpool,leaf_capacity);
	
	hi_tok = threadpool.launchJob(pthreaded_kdsubtree, &hi_arg);
	if (hi_tok == -1) n->right_child = kdsubtree(hi_pts,nextDimension,num_dimensions,threadpool,leaf_capacity);
	
	if (lo_tok != -1) { threadpool.join(lo_tok); n->left_child  = lo_arg.result; }
	if (hi_tok != -1) { threadpool.join(hi_tok); n->right_child = hi_arg.result; }

	return n;
}

Node* Node::build_kdtree(TrainingData& td, ThreadPool& threadpool, size_t leaf_capacity) {
	return kdsubtree(td.points, 0, td.num_dimensions, threadpool, leaf_capacity);
}