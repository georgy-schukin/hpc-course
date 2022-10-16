#include <pthread.h>
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

struct ThreadArgs {
	int id;
	int nt;
	queue<double> *data;
	double *sum;
	pthread_mutex_t *mutex;
};

void* thread_func(void *arg) {
	ThreadArgs *ta = (ThreadArgs*)arg;

	int cnt = 0;

	while (true) {
		pthread_mutex_lock(ta->mutex);
		if (ta->data->empty()) {
			pthread_mutex_unlock(ta->mutex);
			printf("%d did %d elems\n", ta->id, cnt);
			return 0;
		}
		auto elem = ta->data->front();
		ta->data->pop();
		pthread_mutex_unlock(ta->mutex);

		double v = func(elem);
		cnt++;

		pthread_mutex_lock(ta->mutex);
		*(ta->sum) += v;
		pthread_mutex_unlock(ta->mutex);
	}
	return 0;
}

int main(int argc, char **argv) {

	int data_size = (argc > 1 ? stoi(argv[1]) : 1000);
	int nt = (argc > 2 ? stoi(argv[2]) : 1);

	queue<double> data;
	for (int i = 0; i < data_size; i++) {
		data.push(i);
	}

	// Computations
	/*for (int i = 0; i < data_size; i++) {
		data[i] += i * func(1000);
	}*/

	vector<pthread_t> threads(nt);
	vector<ThreadArgs> t_args(nt);

	int block_size = data_size / nt;

	double sum = 0;

	pthread_mutex_t mutex;

	pthread_mutex_init(&mutex, NULL);

	auto ts = hrc::now();
	for (int i = 0; i < nt; i++) {
		t_args[i].id = i;
		t_args[i].nt = nt;
		t_args[i].data = &data;
		t_args[i].sum = &sum;
		t_args[i].mutex = &mutex;
		pthread_create(&threads[i], NULL, &thread_func, &t_args[i]);
	}

	//...

	for (int i = 0; i < nt; i++) {
		pthread_join(threads[i], NULL);
	}
	auto te = hrc::now();

	double time = duration<double>(te - ts).count();

	cout << "Time = " << time << endl;
	cout << "Sum = " << sum << endl;

	/*for (auto &el: data) {
		cout << el << " ";
	}
	cout << endl;*/

	pthread_mutex_destroy(&mutex);

	return 0;
}
