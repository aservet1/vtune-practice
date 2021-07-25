#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <map>
#include <random>
#include <cassert> 
#include <cstring>
#include "MullerRandomPointGenerator.hpp"

#define console_log(msg) { std::cout << msg << std::endl; }
#define array_print(arr, len) { \
	printf("[ "); \
	for (int i = 0; i < len-1; i++)\
		printf("%f, ",arr[i]); \
	printf("%f ]\n", arr[len-1]); \
}

typedef std::map<float, float> Histogram;
typedef std::vector<double> Point;

static double
distance_from_surface(Point& point, int radius) { // radius - distance from origin
	double sqdist = 0;
	for (double coordinate : point)
		sqdist += pow(coordinate,2); // getting distance from center...why no work?? sadface.
	return radius - sqrt(sqdist);
}

// round to 2 decimal places
static inline double
round2(double d) {
	return std::ceil(d/.01)/100;
}

#define N_POINTS_TO_GENERATE 1000000

Histogram
make_hist(int dimension) {
	MullerRandomPointGenerator gen;
	Histogram h;
	Point point(dimension);
	for (int _ = 0; _ < N_POINTS_TO_GENERATE; _++) {
		gen.random_point(point, dimension);
		double d = distance_from_surface(point, 1);
		h[round2(d)]++;
	}
	// normalize to get fraction instead of raw number
	for (auto it = h.begin(); it != h.end(); it++) {
		h[it->first] = it->second / N_POINTS_TO_GENERATE;
	}
	return h;
}

void
print_histogram(Histogram h) {
	for (double i = 0.0; i <= 1.01; i+=0.01)
		std::cout << i << ": " << h[i] << std::endl;
}

void jsonstring(Histogram& h, char buf[]) {
	int offset = 0;
	offset += sprintf(buf+offset,"{");
	double i;
	for (i = 0; i < 1.0; i+=0.01) {
		offset += sprintf(buf+offset,"\"%.2f\":%f,",i,h[i]);
	}
	offset += sprintf(buf+offset,"\"%.2f\":%f}",i,h[i]);
}

void jsondump(std::map<int,Histogram>& histograms, const char* filename) {
	FILE* f = fopen(filename,"w");
	char jsonbuf[4096];
	fprintf(f,"{");
	auto it = histograms.begin();
	auto end = histograms.end()--;
	for (; it != end; it++) {
		jsonstring(histograms[it->first],jsonbuf);
		fprintf(f,"\"%d\":%s,",it->first,jsonbuf);
	}
	jsonstring(histograms[it->first],jsonbuf);
	fprintf(f,"\"%d\":%s}",it->first,jsonbuf);
	fclose(f);
}

void run_sequential() {
	std::map<int,Histogram> histograms;
	for (int dim = 2; dim <= 50; dim++) {
		histograms[dim] = make_hist(dim);
	}
	jsondump(histograms,"out.json");
}

void run_parallel() {
	std::map<int,Histogram> histograms;
	#pragma omp parallel for
		for (int dim = 2; dim <= 50; dim++) {
			histograms[dim] = make_hist(dim);
		}
	jsondump(histograms,"out.json");
}

int main(int argc, char* argv[]) {
	if (argc != 2 &&
			(
				!strcmp(argv[1],"parallel") &&
				!strcmp(argv[1],"sequental")
			)
	) {
		fprintf(stderr,"usage: %s [parallel|sequential]\n",argv[0]);
		exit(2);
	}

	struct timeval start,end,diff;
	gettimeofday(&start,NULL);
	if (!strcmp(argv[1],"parallel"))   run_parallel();
	if (!strcmp(argv[1],"sequential")) run_sequential();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&diff);
	printf("%ld seconds elapsed for %s\n",diff.tv_sec,argv[1]);
	return 0;
}
