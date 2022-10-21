#include <omp.h>
#include <vector>
#include <iostream>
#include <numeric>
#include <string>
#include <cmath>
#include <cstdlib>

using namespace std;

double func(int n) {
	double res = 0;
	for (int i = 0; i < n; i++) {
		res += sin(i) * cos(i) * pow(i + 1, i + 1);
	}
	return res;
}

int main(int argc, char **argv) {

	int nrows = (argc > 1 ? stoi(argv[1]) : 1000);
	int ncols = (argc > 2 ? stoi(argv[2]) : 1000);

	vector<double> matrix(nrows * ncols);
	iota(matrix.begin(), matrix.end(), 0);

	double ts = omp_get_wtime();
	#pragma omp parallel for schedule(dynamic, 10)
	for (int i = 0; i < nrows; i++) {
		for (int j = i; j < ncols; j++) {
			matrix[i * ncols + j] += func(i + 2 * j);
		}
	}
	double te = omp_get_wtime();

	double time = te - ts;

	cout << "Time = " << time << endl;

	/*for (auto &el: data) {
		cout << el << " ";
	}
	cout << endl;*/
}
