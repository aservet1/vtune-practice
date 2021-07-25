#include <queue>
#include <assert.h>
#include <vector>
#include <functional>
#include <bits/stdc++.h>
#include "tree.hpp"
#include "DataTypes.hpp"
#include "knn.hpp"

static float
sqdist(Point a, Point b) { // square of euclidean distance from a to b
    assert(a.size() == b.size());
    float d = 0;
    for (long unsigned int i = 0; i < a.size(); i++) {
        d += (b[i]-a[i]) * (b[i]-a[i]);
    }
    return d;
}

template <typename Q> // template for priority queue
static void
aux_knn(Query query, unsigned int k, Node* root, Q& queue) {
	if (root->type == LEAF) {
		Leaf* n = (Leaf*)root;

		for (Point p : n->points) {
			if (queue.size() < k) queue.push(p);
			else if ( sqdist(p,query) < sqdist(queue.top(),query) ) {
				queue.pop(); queue.push(p);
			}
		}

		return;
	}

	Internal* n = (Internal*)root;

	Node* follow_branch = (query[n->dspliT] <= n->value) ? n->left_child : n->right_child;
	aux_knn(query, k, follow_branch, queue);

	unsigned int dspliT = n->dspliT;
	if (query[dspliT] - n->value  >  query[dspliT] -  queue.top()[dspliT] ) {
		Node* other_branch = (follow_branch == n->left_child) ? n->right_child : n->left_child;
		aux_knn(query, k, other_branch, queue);
	}
	return;
}

std::vector<Query>
knn(Query query, unsigned int k, Node* tree) {
    auto mycmp = [&query](Point a, Point b) {
        return sqdist(a, query) < sqdist(b, query);
    };

    std::priority_queue<Point, std::vector<Point>, decltype(mycmp)> q(mycmp);
	aux_knn(query, k, tree, q);

	std::vector<Point> v;
	while (!q.empty()) {
		v.push_back(q.top());
		q.pop();
	}

	std::sort(v.begin(),v.end(),mycmp);

	// // temporary validation stuff. make sure distances are going to be returned in order
	// std::vector<float> dists;
	// for (Point p : v) dists.push_back(sqdist(p,query));
	// for (int i = 1; i < v.size(); i++) {
	// 	if(dists[i] < dists[i-1]) {
	// 		printf("!!!!\n");
	// 	}
	// 	else printf("xx.\n");
	// }

	return v;
}

std::vector<Point>
inefficient_knn(Query query, int k, std::vector<Point>& points) {
	std::vector<Point> result(k);
	auto mycmp = [&query](Point a, Point b) {
        return sqdist(a, query) < sqdist(b, query);
    };
	std::sort(points.begin(), points.end(), mycmp);
	//std::copy(points.begin(), points.begin()+k, result);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = points[i];
	}
	return result;
}