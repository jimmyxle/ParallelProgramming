/* jimmy */
#include "mpi.h"
#include <stdio.h>


int main(int argc, char* argv[])
{
	int num_tasks, task_id;

	MPI_Init(&argc, &argv);
    MPI_Comm child_comm;

    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    if (task_id == 0)
	    printf("Total tasks in comm_world is: %d\n", num_tasks);
    printf("MPI task %d has started...\n", task_id);
    

	int n[10] = {25, 16, 4, 1,64,81,100,49,36,9};
	printf("Num: %d\n",n[4]);

    MPI_Finalize();

	return 0;
}
