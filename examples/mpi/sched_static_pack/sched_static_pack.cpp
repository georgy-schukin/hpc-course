#include <mpi.h>
#include <vector>
#include <cmath>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <map>

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
	int pack_size = (argc > 2 ? stoi(argv[2]) : 1);

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

	vector<MPI_Request> reqs;
	if (rank == 0) {
		// Init tasks
		for (int i = 0; i < num_of_tasks; i++) {
			tasks.push_back(Task {i});
		}

		// Init tasks map
		map<int, vector<Task>> tasks_map;
		int block_size = num_of_tasks / size;
		for (int i = 0; i < (int)tasks.size(); i++) {
			int dest_rank = i / block_size;//rand() % size;
			tasks_map[dest_rank].push_back(tasks[i]);
		}

		// Distribute tasks
		for (auto &p: tasks_map) {
			int dest_rank = p.first;
			auto &tasks_to_send = p.second;
			int num_of_tasks_to_send = (int)tasks_to_send.size();
			for (int i = 0; i < num_of_tasks_to_send; i += pack_size) {
				// Send a pack of tasks to dest process
				reqs.push_back(MPI_Request {});
				int sz = (i + pack_size < num_of_tasks_to_send ? pack_size : num_of_tasks_to_send - i);
				MPI_Isend(&tasks_to_send[i], sz, task_type, dest_rank, TASK_TAG, MPI_COMM_WORLD, &reqs.back());
			}
		}

		// Sending exit messages
		for (int i = 0; i < size; i++) {
			reqs.push_back(MPI_Request {});
			MPI_Isend(nullptr, 0, MPI_INT, i, EXIT_TAG, MPI_COMM_WORLD, &reqs.back());
		}
	}

	// Recv and execute tasks
	bool flag = true;
	double exec_time = 0;
	int tasks_cnt = 0;
	double ts = MPI_Wtime();
	while (flag) {
		MPI_Status stat;
		MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
		const int tag = stat.MPI_TAG;
		const int source_rank = stat.MPI_SOURCE;
		if (tag == TASK_TAG) {
			int sz;
			MPI_Get_count(&stat, task_type, &sz);
			vector<Task> tasks_to_exec(sz);
			MPI_Recv(tasks_to_exec.data(), sz, task_type, source_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			auto ets = MPI_Wtime();
			for (auto &task: tasks_to_exec) {
				exec_task(task);
			//printf("%d: exec %d\n", rank, task.id);
			}
			exec_time += MPI_Wtime() - ets;
			tasks_cnt += sz;

		} else if (tag == EXIT_TAG) {
			MPI_Recv(nullptr, 0, MPI_INT, source_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			flag = false;
		}
	}
	double time = MPI_Wtime() - ts;

	printf("%d: time = %.5f, exec time = %.5f (%d tasks)\n", rank, time, exec_time, tasks_cnt);

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
