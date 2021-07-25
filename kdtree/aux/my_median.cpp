#include <iostream>
#include <vector>
#include <unistd.h>

/** credit to https://www.codespeedy.com/find-median-of-an-array-using-quick-select-algorithm-in-cpp/
	which was the original copy/paste of this file. same main concept, but i went in and changed
	a bunch of the inner workings to make this method work for my problem in the points array /
	kd tree building situation */

#include "DataTypes.hpp"
#include "my_median.hpp"
//typedef std::vector<float> Point;

static void swap(Point* a, Point* b) {
    Point temp = *a;
    *a = *b;
    *b = temp;
}

static int partition(std::vector<Point>& points, int l, int r, int D) {
    Point lst=points[r];
	int i=l,j=l;
    while (j < r) {
        if (points[j][D] < lst[D]) {
            swap(&points[i], &points[j]);
            i++;
        }
        j++;
    }
    swap(&points[i], &points[r]);
    return i;
}

static int randomPartition(std::vector<Point>& points, int l, int r, unsigned int* seed, int D) {
    int n = r - l + 1;
    int pivot = rand_r(seed) % n;
    swap(&points[l + pivot], &points[r]);
    return partition(points, l, r, D);
}

static void MedianUtil(std::vector<Point>& points, int l, int r, int k,
				Point& a, Point& b, unsigned int* seed, int D) {
    if (l <= r) {
        int partitionIndex = randomPartition(points, l, r, seed, D);
        if (partitionIndex == k) {
            b = points[partitionIndex];
            //if (a != -1) return;
        }
        else if (partitionIndex == k - 1) {
            a = points[partitionIndex];
            //if (b != -1) return;
        }
        if (partitionIndex >= k)
            return MedianUtil(points, l, partitionIndex - 1, k, a, b, seed, D);
        else
            return MedianUtil(points, partitionIndex + 1, r, k, a, b, seed, D);
    }
    return;
}

static Point fMedian(std::vector<Point>& points, unsigned int* seed, int D)  {
    Point ans, a, b;
	int n = points.size();
    MedianUtil(points, 0, (n-1), (n/2), a, b, seed, D);
    ans = b;
    return ans;
}

Point
my_median(std::vector<Point>& points, int dimension) {
	int D = dimension;
	unsigned int seed = time(NULL);
	srand(seed);
	return fMedian(points, &seed, D);
}

// Point randPoint(int d) {
// 	Point p(d);
// 	for (int i = 0; i < d; i++) {
// 		p[i] = rand() % 10;
// 	}
// 	return p;
// }
// 
// #define _printvec(v) std::cout << "( ";							\
// 							for (int i = 0; i < v.size(); i++)	\
// 								std::cout << v[i] << " "; 		\
// 							std::cout << ")" << std::endl;		\
// 
// int main(int argc, char *argv[])
// {
// 
// 	srand(time(NULL));
// 
// 	int dimensions = 4;
// 	int split_dim = 0;
// 
// 	std::vector<Point> points;
// 	for (int x = 0 ; x < 20; x++) {
// 		Point p = randPoint(dimensions);
// 		points.push_back(p);
// 		_printvec(p);
// 	}
// 	std::cout << " === " << std::endl;
// 	
// 	Point m = my_median(points, split_dim);
// 	_printvec(m);
// 
// 	return 0;
// }
