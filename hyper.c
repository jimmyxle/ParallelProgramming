/* jimmy*/
#include <stdio.h>
#include "mpi.h"


int main(int argc, char* argv[])
{
	int num_tasks;
	MPI_Init(&argc, &argv);
	MPI_Comm child_comm;

	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	printf("Total tasks in child_comm is: %d\n", num_tasks);

	int n[10] = {25, 16, 4, 1,64,81,100,49,36,9};
	printf("Num: %d\n",n[4]);
	return 0;
}
