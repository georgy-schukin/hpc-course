#include <mpi.h>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

vector<int> partition(int size, int num) {
	vector<int> sizes(num);
	int base_size = size / num;
	int remainder = size % num;
	for (int i = 0; i < num; i++) {
		sizes[i] = base_size + (i < remainder ? 1 : 0);
	}
	return sizes;
}

vector<int> getShifts(const vector<int> &sizes) {
	int sz = sizes.size();
	vector<int> shifts(sz + 1);
	shifts[0] = 0;
	for (int i = 0; i < sz; i++) {
		shifts[i + 1] = shifts[i] + sizes[i];
	}
	return shifts;
}

int main(int argc, char **argv) {

	MPI_Init(&argc, &argv);

	int nrows = (argc > 1 ? stoi(argv[1]) : 1000);
	int ncols = (argc > 2 ? stoi(argv[2]) : nrows);

	int rank, size;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int dims[2] = {0, 0};
	MPI_Dims_create(size, 2, dims);
	int periodic[2] = {0, 0};

	MPI_Comm cart_comm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 0, &cart_comm);

	MPI_Comm_rank(cart_comm, &rank);

	int coords[2];
	MPI_Cart_coords(cart_comm, rank, 2, coords); 

	auto row_sizes = partition(nrows, dims[0]);
	auto col_sizes = partition(ncols, dims[1]);

	int row_block_size = row_sizes[coords[0]];
	int col_block_size = col_sizes[coords[1]];

	auto row_shifts = getShifts(row_sizes);
	auto col_shifts = getShifts(col_sizes);

	int row_shift = row_shifts[coords[0]];
	int col_shift = col_shifts[coords[1]];

	if (rank == 0) {
		printf("Mesh %dx%d\n", dims[0], dims[1]);
	}
	printf("%d: coord (%d, %d), block %dx%d, shift (%d, %d)\n",
		rank, coords[0], coords[1], row_block_size, col_block_size, row_shift, col_shift);

	vector<double> matrix_block(row_block_size * col_block_size);

	for (int i = 0; i < row_block_size; i++)
	for (int j = 0; j < col_block_size; j++) {
		int gi = i + row_shift;
		int gj = j + col_shift;
		matrix_block[i * col_block_size + j] = gi + gj;
	}

	MPI_Finalize();

	return 0;
}

