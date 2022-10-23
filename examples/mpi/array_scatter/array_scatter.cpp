#include <mpi.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <numeric>

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

	std::vector<double> array;

	std::vector<MPI_Request> reqs;

	if (rank == 0) {
		array.resize(array_size);
		std::iota(array.begin(), array.end(), 0);
		/*reqs.resize(size);

		for (int i = 0; i < size; i++) {
			// send block i to process i
			MPI_Isend(array.data() + i * block_size, block_size, MPI_DOUBLE, i, 100, MPI_COMM_WORLD, &reqs[i]);
		}*/
	}

	std::vector<double> block(block_size);

	MPI_Scatter(array.data(), block_size, MPI_DOUBLE, block.data(), block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// recv block from process i
//	MPI_Recv(block.data(), block_size, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	std::ostringstream out;
	out << rank << ": ";
	for (auto &el: block) {
		out << el << " ";
	}
	out << std::endl;
	std::cout << out.str();

	double ts = MPI_Wtime();
	for (int i = 0; i < block_size; i++) {
		int gi = i + block_start;
		block[i] = block[i] * func(1000);
	}
	double te = MPI_Wtime();

/*	if (rank == 0) {
		for (auto &req: reqs) {
			MPI_Wait(&req, MPI_STATUS_IGNORE);
		}
	}*/

	double time = te - ts;

	MPI_Gather(block.data(), block_size, MPI_DOUBLE, array.data(), block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (auto &el: array) {
			std::cout << el << " ";
		}
		std::cout << std::endl;
	}

	double max_time;

	MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Time = %.5f\n", max_time);
	}

	MPI_Finalize();

	return 0;
}
