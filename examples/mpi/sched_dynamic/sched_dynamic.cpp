#include <mpi.h>
#include <vector>
#include <cmath>
#include <string>
#include <cstdio>
#include <cstdlib>

using namespace std;

struct Task {
	int id;
};

double func(int n) {
	double res = 0;
	for (int i = 0; i < n; i++) {
		res += sin(i) * cos(i) * pow(i + 1, i + 1);
	}
	return 0;
}

void exec_task(const Task &task) {
	func(task.id + 1);
}

int main(int argc, char **argv) {

	MPI_Init(&argc, &argv);

	int num_of_tasks = (argc > 1 ? stoi(argv[1]) : 1000); 

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	vector<Task> tasks;

	MPI_Datatype task_type;
	{
		int lens[1] = {1};
		MPI_Aint displs[1] = {0};
		MPI_Datatype types[1] = {MPI_INT};
		MPI_Datatype tmp_type;
		MPI_Type_create_struct(1, lens, displs, types, &tmp_type);
		MPI_Type_create_resized(tmp_type, 0, sizeof(Task), &task_type);
		MPI_Type_commit(&task_type);
	}

	int TASK_TAG = 100;
	int EXIT_TAG = 101;
	int REQUEST_TAG = 102;

	vector<MPI_Request> reqs;

	double time = 0;
	double exec_time= 0;

	if (rank == 0) {
		// Init tasks
		for (int i = 0; i < num_of_tasks; i++) {
			tasks.push_back(Task {i});
		}

		// Distribute tasks
		int remaining_workers = size - 1;
		int current_task_index = 0;
		while (remaining_workers > 0) {
			MPI_Status stat;
			MPI_Probe(MPI_ANY_SOURCE, REQUEST_TAG, MPI_COMM_WORLD, &stat);
			int dest_rank = stat.MPI_SOURCE;
			reqs.push_back(MPI_Request {});
			MPI_Recv(nullptr, 0, MPI_INT, dest_rank, REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (current_task_index < tasks.size()) {
				// Send task i to dest process
				MPI_Isend(&tasks[current_task_index], 1, task_type, dest_rank, TASK_TAG, MPI_COMM_WORLD, &reqs.back());
				current_task_index++;
			} else {
				MPI_Isend(nullptr, 0, task_type, dest_rank, EXIT_TAG, MPI_COMM_WORLD, &reqs.back());
				remaining_workers--;
			}
		}
	} else {
		// Recv and execute tasks
		bool flag = true;
		int tasks_cnt = 0;
		double ts = MPI_Wtime();
		while (flag) {
			MPI_Send(nullptr, 0, MPI_INT, 0, REQUEST_TAG, MPI_COMM_WORLD);
			MPI_Status stat;
			MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			const int tag = stat.MPI_TAG;
			const int source_rank = stat.MPI_SOURCE;
			if (tag == TASK_TAG) {
				Task task;
				MPI_Recv(&task, 1, task_type, source_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				auto ets = MPI_Wtime();
				exec_task(task);
				exec_time += MPI_Wtime() - ets;
				tasks_cnt++;

				//printf("%d: exec %d\n", rank, task.id);
			} else if (tag == EXIT_TAG) {
				MPI_Recv(nullptr, 0, MPI_INT, source_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				flag = false;
			}
		}
		time = MPI_Wtime() - ts;

		printf("%d: time = %.5f, exec time = %.5f (%d tasks)\n", rank, time, exec_time, tasks_cnt);
	}

	double max_time = 0;
	double max_exec_time = 0;
	MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&exec_time, &max_exec_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Time = %.5f, exec time = %.5f\n", max_time, max_exec_time);
	}

	for (auto &req: reqs) {
		MPI_Wait(&req, MPI_STATUS_IGNORE);
	}

	MPI_Finalize();

	return 0;
}
