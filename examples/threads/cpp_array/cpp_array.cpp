#include <thread>
#include <cstdio>
#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
using hrc = high_resolution_clock;

double func(int n) {
	double r = 0;
	for (int i = 0; i < n; i++) {
		r += sin(i) * cos(i);
	}
	return r;
}

void thread_func(double *data, int start, int end) {
	for (int i = start; i < end; i++) {
		data[i] += func(i);
	}
}

int main(int argc, char **argv) {

	int data_size = (argc > 1 ? stoi(argv[1]) : 1000);
	int nt = (argc > 2 ? stoi(argv[2]) : 1);

	vector<double> data(data_size);
	iota(data.begin(), data.end(), 0);

	vector<thread> threads;

	int block_size = data_size / nt;

	auto ts = hrc::now();
	for (int i = 0; i < nt; i++) {
		thread thread(&thread_func, data.data(), i * block_size, (i + 1) * block_size);
		threads.push_back(move(thread));
	}

	//...

	for (int i = 0; i < nt; i++) {
		threads[i].join();
	}
	auto te = hrc::now();

	double time = duration<double>(te - ts).count();

	cout << "Time = " << time << endl;

	/*for (auto &el: data) {
		cout << el << " ";
	}
	cout << endl;*/

	return 0;
}
