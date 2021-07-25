#include <queue>
#include <vector>
#include <iostream>
#include <cassert>
#include <functional>
 
typedef std::vector<float> Point;

template<typename T>
void print_queue(T q) { // NB: pass by value so the print uses a copy
    while(!q.empty()) {
        std::cout << q.top() << ' ';
        q.pop();
    }
    std::cout << std::endl;;
}

template<typename T>
void print_point_queue(T q) { // NB: pass by value so the print uses a copy
    while(!q.empty()) {
        for (int i = 0; i < q.top().size(); i++)
			std::cout << q.top()[i] << ',';
		std::cout << " ";
        q.pop();
    }
    std::cout << std::endl;
}

float
sqdist(Point a, Point b) { // square of euclidean distance from a to b
    assert(a.size() == b.size());
    float d = 0;
    for (int i = 0; i < a.size(); i++) {
        d += (b[i]-a[i]) * (b[i]-a[i]);
    }
    return d;
}

Point
rndpnt() {
	int size = 3;
	Point p(size);
	for (int i = 0; i < size; i++) {
		p[i] = rand() % 10;
	}
	return p;
}

void function(std::string s) {
	printf("hello world\n");
}

int main() {
    std::priority_queue<int> q;
 
    const auto data = {1,8,5,6,3,4,0,9,7,2};
 
    for(int n : data)
        q.push(n);
 
    print_queue(q);
 
    std::priority_queue<int, std::vector<int>, std::greater<int>>
        q2(data.begin(), data.end());
 
    print_queue(q2);
 
    // Using lambda to compare elements.
    auto cmp = [](int left, int right) { return (left ^ 1) < (right ^ 1); };
    std::priority_queue<int, std::vector<int>, decltype(cmp)> q3(cmp);
 
    for(int n : data)
        q3.push(n);
 
    print_queue(q3);

	Point queryPoint{1,2,3};
	auto mycmp = [&queryPoint](Point a, Point b) {
		return sqdist(a,queryPoint) < sqdist(b,queryPoint);
	};

	std::priority_queue<Point, std::vector<Point>, decltype(mycmp)> qz(mycmp);

	srand(14818);
	std::vector<Point> pts; for (int i = 0; i < 10; i++) qz.push(rndpnt());
	print_point_queue(qz);
	function("");
}




