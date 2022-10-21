#include <omp.h>
#include <vector>
#include <string>
#include <numeric>
#include <iostream>
#include <cmath>
#include <cstdio>

using namespace std;

double func(int n) {
	double res = 0;
	for (int i = 0; i < n; i++) {
		res += sin(i) * cos(i);
	}
	return res;
}

int main(int argc, char **argv) {

	int data_size = (argc > 1 ? stoi(argv[1]) : 1000);

	// Init data
	vector<double> data(data_size);
	iota(data.begin(), data.end(), 0);

	// Do computations
	double ts = omp_get_wtime();
	#pragma omp parallel
	{
		int tid = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		int block_size = data_size / nthreads;
		int start = tid * block_size;
		int end = start + block_size;
		for (int i = start; i < end; i++) {
			data[i] += func(i);
		}
	}
	// implicit barrier
	double te = omp_get_wtime();

	double time = te - ts;

	cout << "Time = " << time << endl;

	for (auto &el: data) {
		//cout << el << " ";
	}
	cout << endl;

	return 0;
}
