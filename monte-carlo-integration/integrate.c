#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define tv_to_usec(tv) \
	(tv.tv_sec * 1000000 + tv.tv_usec)

static float* sum_buffer;

typedef struct {
	float a;
	float b;
	int portion_size;
	int n;
	int idx;
} ArgStruct;

// thanks to https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
// modifying it to be thread safe, though
float
RandomFloat(float a, float b, unsigned int* seed) {
    float random = ((float) rand_r(seed)) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

float
sum_portion(float a, float b, int portion_size, int n) {
	unsigned int seed = time(NULL);
	srand(seed);
	float sum = 0;
	for (int _ = 0; _ < portion_size; _++) {
		float x = RandomFloat(a,b,&seed);
		sum += (1/(float)n)*sin(x)/x;
	} return sum;
}

void* run(void* arg) {
	ArgStruct* args = (ArgStruct*)arg;
	sum_buffer[args->idx] = sum_portion(args->a, args->b, args->portion_size, args->n);
	return NULL;
}

float
integrate(float a, float b, int n, int n_threads, pthread_t* tids) {

	sum_buffer = (float*)malloc(sizeof(float)*n_threads);
	ArgStruct args[n_threads];

	float sum = 0;
	for (int i = 0; i < n_threads; i++) {
		args[i].a = a;
		args[i].b = b;
		args[i].portion_size = n/n_threads;
		args[i].n = n;
		args[i].idx = i;
		pthread_create(&tids[i], NULL, run, &args[i]);
	}
	for (int i = 0; i < n_threads; i++) {
		pthread_join(tids[i], NULL);
		sum += sum_buffer[i];
	}

//	for (int i = 0; i < n_threads; i++) {
//		std::cout << sum_buffer[i] << " : ";
//		std::cout << args[i].idx << std::endl;
//	}
//	std::cout << "---" << std::endl;

	free (sum_buffer);
	return (b-a)*sum;
}

float // sanity check, so we know if the multithreading is altering the results
integrate_nothread(float a, float b, int n) {
	unsigned int seed = time(NULL);
	srand(seed);
	float sum = 0;
	for (int _ = 0; _ < n; _++) {
		float x = RandomFloat(a,b,&seed);
		sum += (1/(float)n)*sin(x)/x;
	}
	return (b-a)*sum;
}

int
main(int argc, char *argv[]) {
	if (argc != 5 && argc != 6) {
		printf("usage: %s a b n n_threads [time data filename]\n", argv[0]);
		printf("  note that if you use an outfile, you're appending to it, not over-writing");
		exit(2);
	}

	float a         =  strtof(argv[1], NULL);
	float b         =  strtof(argv[2], NULL);
	int   n         =  strtof(argv[3], NULL);
	int   n_threads =  strtof(argv[4], NULL);
	char* filename  =  (argc == 6) ? argv[5] : NULL;

	pthread_t tids[n_threads];

	struct timeval start, end;
	gettimeofday(&start, NULL);
	float integral = integrate (
		a, b, n,
		n_threads, tids
	);
	gettimeofday(&start, NULL);
	long time = tv_to_usec(end) - tv_to_usec(start);

	printf("%f\n", integral);

	if (filename) {
		FILE* out = fopen(filename, "a");
		fprintf(out, "%d,%ld\n", n_threads, time);
		fclose(out);
	}
	
	return 0;
}
