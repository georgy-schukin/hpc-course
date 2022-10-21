#include <omp.h>
#include <cstdio>
#include <string>

using namespace std;

int main(int argc, char **argv) {

	int nt = (argc > 1 ? stoi(argv[1]) : 1);

	//omp_set_num_threads(nt);

	#pragma omp parallel num_threads(nt)
	{
		int tid = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		printf("Hello, I am %d from %d\n", tid, nthreads);
	}

	omp_set_num_threads(2);

	#pragma omp parallel
	{
		int tid = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		printf("Hello again, I am %d from %d\n", tid, nthreads);
	}

	return 0;
}
