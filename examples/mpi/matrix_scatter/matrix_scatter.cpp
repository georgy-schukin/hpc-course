#include <mpi.h>
#include <vector>
#include <string>
#include <cstdio>
#include <numeric>
#include <iostream>
#include <sstream>

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

	MPI_Datatype block_type;
	MPI_Datatype block_type_ext;
	MPI_Type_vector(row_block_size, col_block_size, ncols, MPI_DOUBLE, &block_type);
	MPI_Type_create_resized(block_type, 0, sizeof(double), &block_type_ext);
	MPI_Type_commit(&block_type);
	MPI_Type_commit(&block_type_ext);

	vector<double> matrix;

	vector<MPI_Request> reqs;

	vector<int> displs(size);

	for (int i = 0; i < size; i++) {
		int dest_coords[2];
		MPI_Cart_coords(cart_comm, i, 2, dest_coords);
		displs[i] = dest_coords[0] * ncols * row_block_size + dest_coords[1] * col_block_size;
	}

	if (rank == 0) {
		matrix.resize(nrows * ncols);
		iota(matrix.begin(), matrix.end(), 0);

		/*for (int i = 0; i < size; i++) {
			// send block i to process i
			reqs.push_back(MPI_Request {});
			MPI_Isend(matrix.data() + displs[i], 1, block_type, i, 100, cart_comm, &reqs.back());
		}*/
	}

	vector<double> matrix_block(row_block_size * col_block_size);

	MPI_Datatype cont_type;
	MPI_Type_contiguous(row_block_size * col_block_size, MPI_DOUBLE, &cont_type);
	MPI_Type_commit(&cont_type);

	// recv block from the main process
//	MPI_Recv(matrix_block.data(), 1, cont_type, 0, 100, cart_comm, MPI_STATUS_IGNORE);

	vector<int> sizes(size, 1);
	MPI_Scatterv(matrix.data(), sizes.data(), displs.data(), block_type_ext, matrix_block.data(), 1, cont_type, 0, cart_comm);

	ostringstream out;
	out << rank << ": ";
	for (int i = 0; i < row_block_size; i++) {
		for (int j = 0; j < col_block_size; j++) {
			out << matrix_block[i * col_block_size + j] << " ";
		}
		out << ", ";
	}
	out << endl;
	cout << out.str();

	for (int i = 0; i < row_block_size; i++)
	for (int j = 0; j < col_block_size; j++) {
		int gi = i + row_start;
		int gj = j + col_start;
		matrix_block[i * col_block_size + j] = gi + gj;
	}

	for (auto &req: reqs) {
		MPI_Wait(&req, MPI_STATUS_IGNORE);
	}

	MPI_Finalize();

	return 0;
}
