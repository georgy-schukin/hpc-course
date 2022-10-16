#include <thread>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

void thread_func(int id, int nt) {
	printf("Hello, I am %d from %d\n", id, nt);
}

int main(int argc, char **argv) {

	int nt = (argc > 1 ? stoi(argv[1]) : 1);

	vector<thread> threads;

	for (int i = 0; i < nt; i++) {
		thread thread(&thread_func, i, nt);
		threads.push_back(move(thread));
	}

	//...

	for (int i = 0; i < nt; i++) {
		threads[i].join();
	}

	return 0;
}
