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
    
    int num_tasks, task_id,child_id, size, degree = 3, pivot, color;
    int recv_data[10];

    MPI_Init(&argc, &argv);

    MPI_Comm childcomm;

    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    /*Scatter parts of array to each process*/
    if (task_id == 0)
    {
        int n[16] = { 1,11,21,31,41,51,61,71,81,91,101,111,121,131,141,151 };
        size = (sizeof(n)/sizeof(int))/num_tasks;
   
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(n, size, MPI_INT, &recv_data, size, MPI_INT, 0, MPI_COMM_WORLD);
        /*
        printf("Task %d: in mpi_comm_world\t", task_id);

        for (int i = 0; i < size; i++)
        {
            if (recv_data[i] > 0)
                printf("%d\t", recv_data[i]);
        }
        */
    }
    else
    {
        int n;
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(&n, 0, MPI_INT, &recv_data, size, MPI_INT, 0, MPI_COMM_WORLD);

        //printf("Task %d in MPI_comm_world\t", task_id);
        /*
        for (int i = 0; i < size; i++)
        {
            if (recv_data[i] > 0)
                printf("%d\t", recv_data[i]);
        }
        */
    }
    /* END initial scattering*/

    
    /*
        figure out binary
        rank 0 bcast pivot
    */
    int max = 1;
    for (int d = 0; d < max; d++)
    {
        /*
        char* str;
        str = to_binary(task_id, degree);
        char * str_copy = to_binary(task_id, degree);
       
        if (str_copy[d] == '1')
            str_copy[d] = '0';
        else
            str_copy[d] = '1';
        printf(" %s and %s \n", str, str_copy);
        printf("Task: %d binary[%d] : %c\n", task_id,d, str[d]);

        */

        if (d == 0)
        {

            //comm split
            if (task_id == 0 || task_id == 4)
            {
                color = 0;
            }
            else if (task_id == 1 || task_id == 5)
            {
                color = 1;
            }
            else if (task_id == 2 || task_id == 6)
            {
                color = 2;
            }
            else
            {
                color = 3;
            }
            MPI_Comm_split(MPI_COMM_WORLD, color, task_id, &childcomm);
            //printf("comm_world rank:  %d", task_id);

            MPI_Comm_rank(childcomm, &child_id);
//            printf("Color:Rank in childcomm is  %d:%d\n",color, child_id);
            /*
                Split the comm
            
            */
            int data[2];

            if (child_id == 0)
            {
                MPI_Send(&recv_data[0], 1, MPI_INT, 1, 0, childcomm);
                MPI_Send(&recv_data[1], 1, MPI_INT, 1, 0, childcomm);


                MPI_Recv(&data[0], 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);
                MPI_Recv(&data[1], 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);

            }
            else
            {   
                MPI_Recv(&data[0], 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);
                MPI_Recv(&data[1], 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);

                MPI_Send(&recv_data[0], 1, MPI_INT, 0, 0, childcomm);
                MPI_Send(&recv_data[1], 1, MPI_INT, 0, 0, childcomm);

            }
            int working_arr[] = { data[0], data[1], recv_data[0], recv_data[1] };

            if (child_id == 0)
            {

                //printf("task_id: %d binary: %s\n", task_id, str);

                pivot = (sizeof(working_arr)/sizeof(int))/2;
                MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm);

            }
            else
            {

                //printf("task_id: %d binary: %s\n", task_id, str);
                pivot = 0;
                MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm);



            }
            printf("pivot data is: %d in ", working_arr[pivot]);
            printf("arr = %d\t%d\t%d\t%d\n", data[0], data[1], recv_data[0], recv_data[1]);


            /*
                separate array into lower end and higher end
                if smaller than pivot, 
            */
        }
        

        


        


//        free(str);
    }

    MPI_Finalize();
    return 0;
}
