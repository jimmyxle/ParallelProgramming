/*
    Jimmy Le 26546986
    Parts of this code was taken from the class tutorials as well
    as https://computing.llnl.gov/tutorials/parallel_comp/#ExamplesPI
*/
#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#define MASTER 0

int main(int argc, char* argv[])
{
    int task_id, num_tasks, num_points, circle_count;
    double x, y, r, PI, sum_pi;
    clock_t start, end;
    /* Obtain number of tasks and task ID */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    printf("MPI task %d has started...\n", task_id);

    srand(time(NULL));

    //Set up parameters
    num_points = 5000000/num_tasks;
    circle_count = 0;
    x = y = 0.0;
    r = 1.0;
    start = clock();
    /*
        Loop to get count
        1. Generate two random numbers
        2. Figure out if its within the circle
        3. Increment count if it is
    */
    for (int j = 1; j < num_points; j++) {
        x = ((double)rand() / (RAND_MAX));
        y = ((double)rand() / (RAND_MAX));
        if (sqrt(x * x + y * y) <= r) {
            circle_count += 1;
        }
    }
    PI = 4.0 * circle_count / num_points;
    sum_pi =0.0;
    //Send PI result back to get the average pi
    MPI_Reduce(&PI, &sum_pi, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
    //Only one process will print out the results.
    if (task_id == MASTER) {
        end = clock();
        printf("PI is approximately: %f\n", sum_pi / num_tasks);
        printf("Duration: %f s\n", ((double)(end - start) / CLOCKS_PER_SEC));
    }
    MPI_Finalize();

    return 0;
}


