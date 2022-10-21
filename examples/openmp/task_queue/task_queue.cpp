#include <omp.h>
#include <vector>
#include <queue>
#include <string>
#include <cmath>
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

	// Init data
	queue<double> data;
	for (int i = 0; i < data_size; i++) {
		data.push(double(i));
	}

	// Do computations
	double ts = omp_get_wtime();
	double sum = 0;
	#pragma omp parallel
	{
		#pragma omp single
		{
			while (!data.empty()) {
				double elem = data.front();
				data.pop();
				#pragma omp task shared(sum)
				{
					double v = func(elem);
					#pragma omp atomic
					sum += v;
				}
			}
		}
	}
	double te = omp_get_wtime();

	cout << "Time = " << te - ts << endl;
	cout << "Sum = " << sum << endl;

	return 0;
}
