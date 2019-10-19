/* jimmy */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* to_binary(int n, int length)
{
    /* taken from https://www.programmingsimplified.com/c/source-code/c-program-convert-decimal-to-binary */
    int c, d, count;
    char* ptr;
    int y = length;
    count = 0;
    ptr = (char*)malloc(y+1);

    if (ptr == NULL)
        exit(EXIT_FAILURE);
    for (c = (y-1); c >= 0; c--)
    {
        d = n >> c;
        if (d&1)
            *(ptr+count) = 1 + '0';
        else
            *(ptr+count) = 0 + '0';
        count++;
    }
    *(ptr+count)='\0';
    return ptr;
}

int main(int argc, char* argv[])
{
    
    int num_tasks, task_id, size, degree = 3, pivot;
    int recv_data[10];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    /*Scatter parts of array to each process*/
    if (task_id == 0)
    {
        int n[10] = { 25, 16, 4, 1,64,81,100,49,36,9 };
        size = (sizeof(n)/sizeof(int))/num_tasks;
   
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(n, size, MPI_INT, &recv_data, size, MPI_INT, 0, MPI_COMM_WORLD);

        printf("Task %d:\t", task_id);

        for (int i = 0; i < size; i++)
        {
            if (recv_data[i] > 0)
                printf("%d\t", recv_data[i]);
        }
    }
    else
    {
        int n;
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(&n, 0, MPI_INT, &recv_data, size, MPI_INT, 0, MPI_COMM_WORLD);

        printf("Task %d:\t", task_id);
        /*
        for (int i = 0; i < size; i++)
        {
            if (recv_data[i] > 0)
                printf("%d\t", recv_data[i]);
        }
        */
    }
    /* END initial scattering*/

    
    if (task_id == 0)
        printf("\nFOR LOOP STARTS\n");
    /*
        figure out binary
        rank 0 bcast pivot
    */
    for (int d = 0; d < 1; d++)
    {
        char* str;
        str = to_binary(task_id, degree);
        char * str_copy = to_binary(task_id, degree);
       
        if (str_copy[d] == '1')
            str_copy[d] = '0';
        else
            str_copy[d] = '1';
        // #TODO: get process to communicate with process that differs in specifically one bit

        

        printf(" %s and %s \n",str,str_copy);
        

        printf("Task: %d binary[%d] : %c\n", task_id,d, str[d]);

        if (task_id == 0)
        {
            
            //printf("task_id: %d binary: %s\n", task_id, str);

            pivot = (0+size)/2;
            MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
            //printf("pivot data is: %d\n", recv_data[pivot]);



            
        }
        else
        {
            
            //printf("task_id: %d binary: %s\n", task_id, str);

            MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
            //printf("pivot data is: %d\n",recv_data[pivot]);

        }


        free(str);
    }

    MPI_Finalize();
    return 0;
}
