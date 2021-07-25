#include <iostream>
#include <unistd.h>
#include <atomic>

#include "ThreadPool.hpp"

/** 
 * This driver isn't part of my code. Just to probe and test key features. Gets compiled
 * into its own executable with my other files, and only if you specifically as my Makefile
 * to do it. Otherwise you can ignore.
*/

void* sleep_print(void* arg) {
	int sec = *((int*)arg);
	std::cout << 0 << "/" << sec << std::endl;
	for (int s = 1; s <= sec; s++) {
		sleep(1);
		std::cout << s << "/" << sec << std::endl;
	}
	return nullptr;
}

std::atomic<int> counter;
void increment() {
	counter += 1;
}
void* threadwork(void* arg) {
	for (int i = 0; i < 1000000; i++)
		increment();
	return nullptr;
}

int main(int argc, char** argv) {
	ThreadPool threadpool(2);
	int mytid1 = threadpool.launchJob(threadwork, nullptr);
	int mytid2 = threadpool.launchJob(threadwork, nullptr);
	threadpool.join(mytid1);
	threadpool.join(mytid2);

	std::cout << counter << std::endl;

	return 0;
}

