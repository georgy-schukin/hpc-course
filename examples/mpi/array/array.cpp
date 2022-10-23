#include <mpi.h>
#include <vector>
#include <string>
#include <cmath>

double func(int n) {
	double res = 0;
	for (int i = 0; i < n; i++) {
		res += std::sin(i) * std::cos(i);
	}
	return res;
}

int main(int argc, char **argv) {

	MPI_Init(&argc, &argv);

	const int array_size = (argc > 1 ? std::stoi(argv[1]) : 100);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int block_size = array_size / size;
	int block_start = rank * block_size;

	std::vector<double> block(block_size);

	double ts = MPI_Wtime();
	for (int i = 0; i < block_size; i++) {
		int gi = i + block_start;
		block[i] = gi * func(1000);
	}
	double te = MPI_Wtime();

	double time = te - ts;

	double max_time;

	MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Time = %.5f\n", max_time);
	}

	MPI_Finalize();

	return 0;
}
