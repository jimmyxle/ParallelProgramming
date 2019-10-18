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
#include <math.h>

#define MASTER 0

int main(int argc, char* argv[])
{
    /*initialize variables*/
    int num_points, circle_count, parent_id;
    double x, y, r, PI, sum_pi;
    int	taskid, numtasks;
    
    MPI_Comm parent;
    
    /*Initialize MPI*/
    MPI_Init(&argc, &argv);
    /*Get the parent comm world so we can send the results back to them.*/
    MPI_Comm_get_parent(&parent);
    MPI_Comm_rank(parent, &parent_id);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    printf("Child task %d has started with process id: %d \n", taskid, getpid());

    srand(time(NULL));

    //Set up parameters
    num_points = 5000000 / numtasks;
    circle_count = 0;
    x = 0.0;
    y = 0.0;
    r = 1.0;
    /*Calculate PI*/
    for (int j = 1; j < num_points; j++) {
        x = ((double)rand() / (RAND_MAX));
        y = ((double)rand() / (RAND_MAX));
        if (sqrt(x * x + y * y) <= r) {
            circle_count += 1;
        }
    }
    PI = 4.0 * circle_count / num_points;
    sum_pi = 0.0;
    //Call reduction function and give it the parent's comm world.
    MPI_Reduce(&PI, &sum_pi, 1, MPI_DOUBLE, MPI_SUM, 0, parent);
    MPI_Finalize();
    return 0;
}
