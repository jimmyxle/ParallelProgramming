/*
    Jimmy Le 26546986
    Parts of this code was taken from the class tutorials as well
    as https://computing.llnl.gov/tutorials/parallel_comp/#ExamplesPI
*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MASTER 0

int main(int argc, char* argv[])
{
    /*Declare variables*/
    int num_points, circle_count;
    double x, y, r, PI, sum_pi;
    clock_t start, end;
    int numtasks, taskid;
    MPI_Comm child_processes;
    /*Initialize MPI*/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    printf("Parent task id is: %d. Total tasks in parent comm are: %d \n", taskid, numtasks);

    if (argc != 2)
        printf("usage: %s <number of workers>\n", argv[0]);
    else
        numtasks = atoi(argv[1]);

    start = clock();
    /*Spawn workers and do work*/
    MPI_Comm_spawn("pi_slave.o", argv, numtasks, MPI_INFO_NULL, 0, MPI_COMM_SELF, &child_processes, MPI_ERRCODES_IGNORE);
    
    MPI_Reduce(&PI, &sum_pi, 1, MPI_DOUBLE, MPI_SUM, MPI_ROOT, child_processes);
    //Only one process will print out the results.
    if (taskid == MASTER) {
        end = clock();
        printf("PI is approximately: %f\n", sum_pi/numtasks);
        printf("Duration: %f s\n", ((double)(end - start) / CLOCKS_PER_SEC));
    }

    MPI_Finalize();
    return 0;
}
