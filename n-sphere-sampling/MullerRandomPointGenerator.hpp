#pragma once
#include <random>
#include <cmath>
#include <vector>

class MullerRandomPointGenerator {

	private:
		std::default_random_engine normal_generator;
		std::normal_distribution<double> normal_distribution;

		std::random_device rd;
		std::mt19937 e2;
		std::uniform_real_distribution<double> uniform_distribution;
	
		double rand_norm() {
			return normal_distribution(normal_generator);
		}

		double rand_uniform() {
			return uniform_distribution(e2);
		}
	
		void norm_range(std::vector<double>& buf, int len) {
			for (int i = 0; i < len; i++)
				buf[i] = rand_norm();
		}

		static double sqrt_squared_sum(std::vector<double>& buf, int len) {
			double sum = 0;
			for (int i = 0; i < len; i++)
				sum += pow(buf[i], 2);
			return sqrt(sum);
		}

	public:
		MullerRandomPointGenerator() {
			normal_distribution = std::normal_distribution<double>(0,1);
			e2 = std::mt19937(rd());
			uniform_distribution = std::uniform_real_distribution<double>(0, 1);
		}

		void random_point(std::vector<double>& point, int dim) {
			std::vector<double> u(dim);
			norm_range(u, dim);
			double norm = sqrt_squared_sum(u, dim);
			double r = pow(rand_uniform(), (1.0/(double)dim));
			for (int i = 0; i < dim; i++)
				point[i] = r*(u[i]/norm);
		}

		static bool verify(std::vector<double>& point, int dim) {
			return sqrt_squared_sum(point, dim) <= 1.0;
		}
};
