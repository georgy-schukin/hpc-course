#include <thread>
#include <mutex>
#include <cstdio>
#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <chrono>
#include <queue>

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

void thread_func(int id, queue<double> &data, double &sum, mutex &mut) {
	while (true) {
		double elem;
		{
			lock_guard<mutex> lock(mut);
			if (data.empty()) {
				return;
			}
			elem = data.front();
			data.pop();
		}

		auto v = func(elem);
		//printf("%d exec %d\n", id, int(elem));

		{
			lock_guard<mutex> lock(mut);
			sum += v;
		}
	}
}

int main(int argc, char **argv) {

	int data_size = (argc > 1 ? stoi(argv[1]) : 1000);
	int nt = (argc > 2 ? stoi(argv[2]) : 1);

	queue<double> data;
	for (int i = 0; i < data_size; i++) {
		data.push(i);
	}

	vector<thread> threads;

	int block_size = data_size / nt;

	double sum = 0;

	mutex mut;

	auto ts = hrc::now();
	for (int i = 0; i < nt; i++) {
		thread thread(&thread_func, i, ref(data), ref(sum), ref(mut));
		threads.push_back(move(thread));
	}

	//...

	for (int i = 0; i < nt; i++) {
		threads[i].join();
	}
	auto te = hrc::now();

	double time = duration<double>(te - ts).count();

	cout << "Time = " << time << endl;
	cout << "Sum = " << sum << endl;

	/*for (auto &el: data) {
		cout << el << " ";
	}
	cout << endl;*/

	return 0;
}
