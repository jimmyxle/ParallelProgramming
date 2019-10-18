/*
    Jimmy Le 26546986
    Parts of this code was taken from the class tutorials as well
    as https://computing.llnl.gov/tutorials/parallel_comp/#ExamplesPI
*/
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

int main(void)
{
    //Set up parameters
    int num_points = 50000000;
    int circle_count = 0;
    double x ,y;
    double  r = 1.0;
    srand(time(NULL));
    clock_t start, end;

    //Start timer
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
    //Calculate PI  
    double PI = 4.0 * circle_count / num_points;

    end= clock();
    double elapsed = ((double)(end - start)  );
    printf("PI is approximately: %f\n", PI);
    printf("Duration: %0.f s\n", elapsed/CLOCKS_PER_SEC);

    return 0;
}


