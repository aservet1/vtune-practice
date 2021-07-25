#pragma once

#include <iostream>
#include <vector>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <atomic>

typedef int ThreadToken;  // value passed around to identify a certain set of thread params that correspond to the thread pool. only valid as long as the thread itself is valid
// it's possible that im mis-using the official definition of the work token

class ThreadPool {
	private:
		static const int MAX_THREADS = 127; // sort of arbitrary value, i wont get near this size
		static const int INACTIVE = 0;
		static const int ACTIVE = 1;

		int nThreads;

		unsigned int seeds[MAX_THREADS];
		int lastSeedIdx = 0; // TODO explain what's going on with how I get random numbers out of this

		pthread_t tids[MAX_THREADS];
		std::atomic<std::intptr_t> active[MAX_THREADS];
		pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

		bool stop_giving_threads;

	public:
		ThreadPool(int n) : nThreads{n} {
			assert(nThreads < MAX_THREADS && nThreads > 0);
			for (int i = 0; i < nThreads; i++) {
				active[i] = INACTIVE;
				seeds[i] = i + time(NULL); // unique seed per thread
			}
			stop_giving_threads = false;
		}
		int total_threads();
		unsigned int* seedptr(ThreadToken tok);
		ThreadToken launchJob(void*(*start)(void*), void *arg);
		void join(ThreadToken threadToken);
		int randomNumber(/*ThreadToken t*/); // thread safe random number
		void reset();
};
