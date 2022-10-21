#include <omp.h>
#include <iostream>
#include <cstdio>

void read_file() {
	printf("read file - %d\n", omp_get_thread_num());
}

void access_database() {
	printf("access database - %d\n", omp_get_thread_num());
}

void do_computations() {
	printf("do computations - %d\n", omp_get_thread_num());
}


int main(int argc, char **argv) {

	#pragma omp parallel
	{
		#pragma omp sections nowait
		{
			#pragma omp section
			read_file();

			#pragma omp section
			access_database();

			#pragma omp section
			do_computations();
		}
		// implicit barrier

		#pragma omp single nowait
		printf("Output - %d\n", omp_get_thread_num());
		// implicit barrier

		#pragma omp single
		printf("Another Output - %d\n", omp_get_thread_num());
		// implicit barrier
	}
	// implicit barrier

	return 0;
}
