#include <mpi.h>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

int main(int argc, char **argv) {

	MPI_Init(&argc, &argv);

	int nrows = (argc > 1 ? stoi(argv[1]) : 1000);
	int ncols = (argc > 2 ? stoi(argv[2]) : nrows);

	int rank, size;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm cart_comm;
	int dims[2] = {0, 0};
	MPI_Dims_create(size, 2, dims);
	int periodic[2] = {0, 0};

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 0, &cart_comm);

	MPI_Comm_rank(cart_comm, &rank);

	int coords[2];
	MPI_Cart_coords(cart_comm, rank, 2, coords);

	int row_block_size = nrows / dims[0];
	int col_block_size = ncols / dims[1];

	int row_start = coords[0] * row_block_size;
	int col_start = coords[1] * col_block_size;

	if (rank == 0) {
		printf("Mesh %dx%d\n", dims[0], dims[1]);
	}
	printf("%d: coord (%d, %d), block %dx%d, shift (%d, %d)\n", 
		rank, coords[0], coords[1], row_block_size, col_block_size, row_start, col_start);

	vector<double> matrix_block(row_block_size * col_block_size);

	for (int i = 0; i < row_block_size; i++)
	for (int j = 0; j < col_block_size; j++) {
		int gi = i + row_start;
		int gj = j + col_start;
		matrix_block[i * col_block_size + j] = gi + gj;
	}

	MPI_Finalize();

	return 0;
}
