#include <omp.h>
#include <cstdio>
#include <unistd.h>

int main(int argc, char **argv) {

	int tid = -1;

	#pragma omp parallel default(none) private(tid)
	{
		tid = omp_get_thread_num();
		usleep(100);
		printf("tid = %d\n", tid);
	}

	printf("Initial tid = %d\n", tid);

	return 0;
}
