#include <pthread.h>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

struct ThreadArgs {
	int id;
	int nt;
};

void* thread_func(void *arg) {
	ThreadArgs *ta = (ThreadArgs*)arg;
	printf("Hello, I am %d from %d\n", ta->id, ta->nt);
	return 0;
}

int main(int argc, char **argv) {

	int nt = (argc > 1 ? stoi(argv[1]) : 1);

	vector<pthread_t> threads(nt);
	vector<ThreadArgs> t_args(nt);

	for (int i = 0; i < nt; i++) {
		t_args[i].id = i;
		t_args[i].nt = nt;
		pthread_create(&threads[i], NULL, &thread_func, &t_args[i]);
	}

	//...

	for (int i = 0; i < nt; i++) {
		pthread_join(threads[i], NULL);
	}

	return 0;
}
