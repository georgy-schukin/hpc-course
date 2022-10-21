#include <omp.h>
#include <vector>
#include <cmath>
#include <string>
#include <numeric>
#include <iostream>

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

	vector<double> data(data_size);
	iota(data.begin(), data.end(), 0);

	double sum = 0;

	double ts = omp_get_wtime();
	#pragma omp parallel for reduction(+ : sum)
	for (int i = 0; i < data_size; i++) {
		data[i] += i * func(1000);
		//#pragma omp atomic
		sum += data[i];
	}
	double te = omp_get_wtime();

	double time = te - ts;

	cout << "Sum = " << sum << endl;
	cout << "Time = " << time << endl;

	return 0;
}
