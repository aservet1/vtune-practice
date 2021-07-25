#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

int main(int argc, char *argv[])
{
	auto lambda = [](int a, int b) {
		return a < b;
	};

	std::vector<int> v = {2,3,987,4,98,32,4};
	for (int x : v) std::cout << x << std::endl;
	std::sort(v.begin(), v.end(), lambda);
	std::cout << std::endl;
	for (int x : v) std::cout << x << std::endl;
	return 0;
}
