#include <omp.h>
#include <vector>
#include <cstdio>
#include <numeric>
#include <string>
#include <cmath>
#include <queue>
#include <list>
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

	list<double> data;
	for (int i = 0; i < data_size; i++) {
		data.push_back(double(i));
	}

	double s = 0;
	#pragma omp parallel
	{
		#pragma omp single
		{
			for (auto &elem: data) {
				#pragma omp task
				{
					printf("%d exec task %d\n", omp_get_thread_num(), (int)elem);
					s += func(elem);
				}
			}
		}
	}

	cout << s << endl;

	return 0;
}
