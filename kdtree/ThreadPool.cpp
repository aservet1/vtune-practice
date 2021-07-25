#include <iostream>
#include <vector>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <atomic>
#include "ThreadPool.hpp"

int ThreadPool::randomNumber(/*ThreadToken t*/) {
	lastSeedIdx = (lastSeedIdx + 1) % nThreads;
	return rand_r(&seeds[lastSeedIdx]);
}

int
ThreadPool::total_threads() {
	return nThreads;
}

// void
// ThreadPool::reset() {
// 	for (int i = 0; i < active.size(); i++)
// 		active[i] = INACTIVE;
// 	stop_giving_threads = false;
// }

ThreadToken
ThreadPool::launchJob(void*(*start)(void*), void *arg) { // rc < 0 means no threads available
	if (stop_giving_threads) return -1;

	ThreadToken threadToken = 0;
	pthread_mutex_lock(&lock);
		while (threadToken < nThreads) {
			if (active[threadToken] == INACTIVE) {
				active[threadToken] = ACTIVE;
				break;
			}
			threadToken++;
		}
	pthread_mutex_unlock(&lock);
	if (threadToken == nThreads) {
		stop_giving_threads = true;
		return -1;
	}

	if (0 != pthread_create(&tids[threadToken], NULL, start, arg)) {
		fprintf(stderr,
			" <-- in create(). errno: %d, strerror: %s\n",
				errno, strerror(errno)
		); exit(errno);
	};
	return threadToken;
}

void
ThreadPool::join(ThreadToken threadToken) {
	assert(threadToken < nThreads); assert(threadToken >= 0); assert(active[threadToken]);
	if (0 != pthread_join(tids[threadToken], NULL)) {
		fprintf(stderr,
			" <-- in join(). errno: %d, strerror: %s\n",
				errno, strerror(errno)
		); exit(errno);
	}
	active[threadToken] = INACTIVE;
}