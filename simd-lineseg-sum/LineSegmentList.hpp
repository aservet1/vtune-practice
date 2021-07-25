#pragma once

// list of 4d line segments. struct of arrays format, for easy parallelization
struct LineSegmentList_4d {
		std::vector<double> x_start, y_start, z_start, q_start;
		std::vector<double> x_stop, y_stop, z_stop, q_stop;

		LineSegmentList_4d(int N) {
			x_start = std::vector<double>(N);
			y_start = std::vector<double>(N);
			z_start = std::vector<double>(N);
			q_start = std::vector<double>(N);
			
			x_stop = std::vector<double>(N);
			y_stop = std::vector<double>(N);
			z_stop = std::vector<double>(N);
			q_stop = std::vector<double>(N);
		}
};
