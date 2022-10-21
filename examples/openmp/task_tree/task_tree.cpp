#include <omp.h>
#include <iostream>
#include <list>
#include <cmath>
#include <cstdlib>
#include <numeric>
#include <vector>
#include <algorithm>

using namespace std;

struct Node {
	double value = 0;
	Node *left = nullptr;
	Node *right = nullptr;

	Node(double v, Node *l = nullptr, Node *r = nullptr) :
		value(v), left(l), right(r) {}
};

void insert(Node *root, double value) {
	Node *curr = root;
	while (curr) {
		if (value < curr->value) {
			if (!curr->left) {
				curr->left = new Node(value);
				return;
			} else curr = curr->left;
		} else {
			if (!curr->right) {
				curr->right = new Node(value);
				return;
			} else curr = curr->right;
		}
	}
}

double func(int n) {
	double v = 0;
	for (int i = 0; i < n; i++) {
		v += sin(i) * cos(i);
	}
	return v;
}

vector<int> cnt(omp_get_max_threads(), 0);

double traverse(Node *root) {
	if (!root) return 0;

	double v1 = 0, v2 = 0;

	if (root->left) {
		#pragma omp task shared(v1) firstprivate(root)
		v1 = traverse(root->left);
	}

	if (root->right) {
		#pragma omp task shared(v2) firstprivate(root)
		v2 = traverse(root->right);
	}

	double v = func(root->value);

	#pragma omp taskwait

	//cout << omp_get_thread_num() << " ";
 	cnt[omp_get_thread_num()]++;

	return v + v1 + v2;
}

int main(int argc, char **argv) {
	int data_size = (argc > 1 ? stoi(argv[1]) : 1000);

	Node *root = new Node(0);

	vector<double> data(data_size);
	iota(data.begin(), data.end(), 1);
	random_shuffle(data.begin(), data.end());

	for (auto el: data) {
		insert(root, el);
	}

	double ts = omp_get_wtime();
	double sum;

	#pragma omp parallel
	{
		#pragma omp single
		{
			sum = traverse(root);
		}

        int tid = omp_get_thread_num();
        printf("Thread %d did %d tasks\n", tid, cnt[tid]);
	}

	double te = omp_get_wtime();
	cout << "Time = " << te - ts << endl;
	cout << "Sum = " << sum << endl;
	return 0;
}
