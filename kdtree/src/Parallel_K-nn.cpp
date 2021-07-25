#include <iostream>
#include <queue>
#include <cmath>
#include <map>
#include "ThreadPool.hpp"
#include "DataTypes.hpp"
#include "errors.hpp"
#include "knn.hpp"

#define clear_bit(bitset, i) bitset &= ~(1<<i);
#define set_bit(bitset, i) bitset |= 1<<i;

static float
dist(Point a, Point b) { // square of euclidean distance from a to b
    if(a.size() != b.size()) {
		printf("in dist, a and b are not same size.\n a: ");
		PointQueryPrint(a); printf("b: "); PointQueryPrint(b);
		printf("\n");
		exit(1);
	}
    float d = 0;
    for (long unsigned int i = 0; i < a.size(); i++) {
        d += (b[i]-a[i]) * (b[i]-a[i]);
    }
    return d;
}

typedef std::queue<Node*> NodeQueue;

#define NUM_NODE_QUEUES 5 // must be less than sizeof(int), to fit in nonEmptyQueues bitset
#define NUM_THREADS 4 //will probably boost this number up

static std::vector< std::atomic<bool> > inUseQueues(NUM_NODE_QUEUES); // this is just so you skip over it, so you don't have to wait for the lock
static std::vector<NodeQueue> nodeQueues(NUM_NODE_QUEUES);
static NodeQueue lowPriorityQueue;
static std::vector<pthread_mutex_t> locks(NUM_NODE_QUEUES);
pthread_mutex_t lowprioritylock;
bool lowpriEmpty = true;
std::atomic<int> nonEmptyQueues; // the underlying 'int' storage is a bit set

template <typename Q>
static bool
isCandidate(Node* _n, Query query, Q pqueue) {
	//printf("in isCandidate()\n");
	if (pqueue.empty()) return true;
	Point furthestNearPoint = pqueue.top();
	Internal* n = (Internal*)_n;
    return abs(n->value - query[n->dspliT]) > abs(n->value - furthestNearPoint[n->dspliT]);
}

static Node*
pollNodeQueues(ThreadPool& threadpool) {
	//printf("in pollNodeQueues()\n");
	if (nonEmptyQueues == 0) {
		pthread_mutex_lock(&lowprioritylock);
		  Node* n = lowPriorityQueue.front();
		  lowPriorityQueue.pop();
		  lowpriEmpty = lowPriorityQueue.empty();
		pthread_mutex_unlock(&lowprioritylock);
		return n;
	}
	int i;
	for(i = threadpool.randomNumber() % nodeQueues.size();
		nodeQueues[i].empty() || inUseQueues[i];
		i = (i+1)%nodeQueues.size()
	); // start at a random index, keep scanning over all of them till you find one that you can use
	pthread_mutex_lock(&locks[i]);
	  inUseQueues[i] = true;
	  Node* n = nodeQueues[i].front(); //TODO consider your 'add in a nullpointer' gimmick if the problem you had im mind arises (-- seems very hacky though)
	  nodeQueues[i].pop();
	  if (nodeQueues[i].empty()) clear_bit(nonEmptyQueues,i);
	  inUseQueues[i] = false;
	pthread_mutex_unlock(&locks[i]);
	return n;
}

static void addToNodeQueue(Node* node, ThreadPool& threadpool) {
	//printf("in addToNodeQueue()\n");
	int i = threadpool.randomNumber()%nodeQueues.size();
	pthread_mutex_lock(&locks[i]);
	  inUseQueues[i] = true;
	  nodeQueues[i].push(node);
	  set_bit(nonEmptyQueues, i);
	  inUseQueues[i] = false;
	pthread_mutex_unlock(&locks[i]);
}

std::vector<Point>
per_thread_work(int k, Query query, ThreadPool& threadpool) {

	std::vector<Point> resultList;
	static auto cmp = [&query](Point a, Point b) { // TODO -- is this ordering in the right direction?
		return dist(a, query) < dist(b, query);
	};
	std::priority_queue<Point, std::vector<Point>, decltype(cmp)> pqueue(cmp);
	while ( ! nonEmptyQueues==0x00000000 || ! lowpriEmpty ) {
		//printf("in another loop iteration\n");
		Node* node = pollNodeQueues(threadpool);
		if (! (node->type == LEAF) ) {
			Internal* n = (Internal*)node;
			if (isCandidate(n->left_child, query, pqueue)) {
				addToNodeQueue(n->left_child, threadpool);
			}
			if (isCandidate(n->right_child, query, pqueue)) {
				addToNodeQueue(n->right_child, threadpool);
			}
            //- add the logic for putting this in the low priority queue
		} else {
			for(Point p : ((Leaf*)node)->points) {
				if (pqueue.size() < (size_t)k) pqueue.push(p);
				else if ( dist(p,query) < dist(pqueue.top(),query) ) {
					pqueue.pop(); pqueue.push(p);
				}
			}
		}
	}
	while (!pqueue.empty()) {
		resultList.push_back(pqueue.top());
		pqueue.pop();
	}
	return resultList;
}

struct pThreadArg {
	int k;
    std::vector<Point> resultList;
    Query query;
    ThreadPool* threadpool;
};

void* run(void* _arg) {
    pThreadArg* args = (pThreadArg*)_arg;
    args->resultList = per_thread_work(args-> k, args->query, *args->threadpool);
    return nullptr;
}

// //std::queue<Node*>
// void
// initialize_nodequeues(Node* root, Query query, ThreadPool& tp) { // fills the node queues with nodes from the original path down to the first closest leaf node
// 	Node* n = root;
// 	while(n->type != LEAF) {
// 		addToNodeQueue(n, tp);
// 		Internal* i = (Internal*)n;
// 		n = (query[i->dspliT] <= i->value) ? i->left_child : i->right_child;
// 	}
// 	addToNodeQueue(n,tp);
// } // mayybe this is a slowdown and i can just initialize by pushing root on, and be fine?

std::vector<Point>
parallel_knn(Query query, int k, Node* root) {
	nonEmptyQueues= 0x00000000; // empty bit set
    for (pthread_mutex_t lock : locks) lock = PTHREAD_MUTEX_INITIALIZER;
	for (int i = 0; i < NUM_NODE_QUEUES; i++ ) inUseQueues[i] = false;
    ThreadPool threadpool(NUM_THREADS);
	// initialize_nodequeues(root, query, threadpool);
	nodeQueues[0].push(root); set_bit(nonEmptyQueues,0);
    std::vector<ThreadToken> tokens(NUM_THREADS);
    std::vector<pThreadArg> args(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
		args[i].k = k;
        args[i].query = query;
        args[i].threadpool = &threadpool;
        tokens[i] = threadpool.launchJob(run, &args[i]);
    }
	for (ThreadToken token : tokens) {
		threadpool.join(token);
	}
	std::vector<Point> finalResult;
	for (pThreadArg arg : args) {
		std::vector<Point> resultList = arg.resultList;
		finalResult.reserve(finalResult.size() + resultList.size());
		finalResult.insert(finalResult.end(), resultList.begin(), resultList.end());
	}

	static auto cmp = [&query](Point a, Point b) {
		return dist(a, query) < dist(b, query);
	};
	std::sort(finalResult.begin(), finalResult.end(), cmp);
	finalResult.resize(k);
	return finalResult;
}