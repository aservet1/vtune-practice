#pragma once

#include <cmath>
#include <vector>
#include <cassert>
#include <iomanip>

typedef std::vector<double> Point;

#define RANDOM(lo, hi) ( (double)(rand_r(seed)%hi) + (double)lo )

static Point
random_point(int dimension, unsigned int* seed) {
	Point p(dimension);
	for (size_t i = 0; i < p.size(); i++)
		p[i] = RANDOM(-100,100) / RANDOM(-100,100); // TODO come up with better bounds for the random number
	return p;
}

class LineSegment {
	private:
		Point a; Point b;

	public:
		LineSegment() {
		}

		LineSegment(Point a, Point b)
			: a{a}, b{b}
		{
			assert(a.size() == b.size() );
		}

		double len() {
			double d = 0; for (size_t i = 0; i < a.size(); i++)
				d += pow(a[i]-b[i], 2);
			return sqrt(d);
		}

		static LineSegment random(int dimension, unsigned int* seed) {
			return LineSegment(random_point(dimension, seed), random_point(dimension, seed));
		}
		
		friend std::ostream& operator<<(std::ostream& os, const LineSegment& ls);
};

std::ostream& operator<<(std::ostream& os, const LineSegment& ls) {
	os << std::setprecision(4);
	os << "(\t";
		for (double coord : ls.a) os << coord << ",\t";
		os << "); (\t";
		for (double coord : ls.b) os << coord << ",\t";
	os << ")";
	return os;
}
