#include <vector>
#include <iostream>

// cache for values
// index i is equivalent to the n in n-sphere where an n-sphere is n+1 dimensional
// for example volume[4] holds the volume area for a 5 dimensional sphere
double volume[20];
double surface_area[20];
// formulas from https://en.wikipedia.org/wiki/N-sphere#Volume_and_surface_area
void initialize_volume_and_surface_area() {
	double* V = volume;
	double* S = surface_area;
	V[0] = 1; S[0] = 2;
	for (int n = 0; n < 20-1; n++) {
		V[n+1] = S[n]/(n+1);
		S[n+1] = 2* M_PI * V[n];
	}
} // this might have ended up not being necessary




class Histogram {
	private:
		std::map<float, int>;

	public:
		Histogram(double lowest, int highest, int step) {
			for (double x = lowest, x <= highest; x+=step) {
				map[x] = 0;
			}
		}

		

};
// #pragma once
// #include <vector>
// 
// class UnitSpherePoint {
// 	private:
// 		std::vector<double> coordinates;
// 	public:
// 		
// };
