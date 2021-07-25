#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
 
typedef std::vector<float> Point;

#define printpoints(p) \
	for (Point p : p) { \
		printpoint(p)	\
	} std::cout << std::endl;

#define printpoint(p) \
	std::cout << "( "; \
	for (int i = 0; i < p.size(); i++) \
		std::cout << p[i] << ", "; \
	std::cout << ")" << std::endl;

Point median(std::vector<Point>& v, int dimension) {
	auto mycmp = [dimension](Point a, Point b) {
		return a[dimension] < b[dimension];
	};
	int median_offset = v.size() / 2;
	std::nth_element(v.begin(), v.begin()+median_offset, v.end(), mycmp);
	return v[median_offset];
}

int main() {

	std::vector<Point> v = {
	    Point{ -370.365, -562.309  },
        Point{ 517.021, 99.0604  },
        Point{ 224.79, 196.642  },
        Point{ 138.559, 705.057  },
        Point{ -637.537, -319.891  },
        Point{ 56.4585, 875.151  },
        Point{ -293.295, 850.19  },
        Point{ -189.955, 548.072  },
        Point{ -403.79, 529.884  },
        Point{ -891.773, -50.2716  },
        Point{ 609.092, 604.213  },
        Point{ 12.2447, 259.677  },
        Point{ -98.7217, 778.418  },
        Point{ -162.108, 829.086  },
        Point{ -169.266, 936.698  },
	};

	printpoints(v);
	Point m = median(v,0);
	printpoints(v);
	printpoint(m); 

	return 0;
}


