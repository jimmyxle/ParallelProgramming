/* jimmy */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 #include <stdio.h>
 #include <time.h>
 #include <math.h>

 void swap_num(int* low, int* high)
 {
     int temp = *low;
     *low = *high;
     *high = temp;
 }

void qusort(int arr[], int q, int r)
{
    int x, s;
    if (q < r)
    {
        x = arr[q];
        s = q;
        for (int i = q+1; i <= r; i++)
        {
            if(arr[i]<=x)
            {
                s = s + 1;
                swap_num(&arr[s], &arr[i]);
            }
        }
        swap_num(&arr[q], &arr[s]);
        qusort(arr, q, s);
        qusort(arr, s+1, r);
    }
}



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
int get_size(int arr[])
{
    int count = 0;

    for (int j = 0; j < 10; j++)
    {

        if (arr[j] > 0 && arr[j]<1000) /*linux has junk info that is negative or some large number*/
        { 
            count++;
        }
        else
        { 
            return count;
        }
    }
    return count;
}

int main(int argc, char* argv[])
{
    int num_tasks, global_id, task_id,child_id, size, degree, pivot, color_degree_1, num_elements =0;
    int *recv_buf, recv_data[10], upper[10], lower[10], recv_n[16], recvcounts[8], displs[8];
    int* active_data, *array_to_sort, mask, bcast_mask, color, bcast_color;
    int *temp, count_element, *data;
    int lower_count, upper_count;
    time_t t;

    /* Intializes random number generator */
    srand((unsigned)time(&t));

    MPI_Init(NULL, NULL);

    MPI_Comm childcomm, childcomm_degree_1, childcomm_degree_2;

    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_rank(MPI_COMM_WORLD, &global_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    degree = log(num_tasks)/log(2); /* 8 p = 3d*/
    /*Scatter parts of array to each process*/
    
    if (argc != 2)
        printf("usage: %s <number of workers>\n", argv[0]);
    else
        num_elements = atoi(argv[1]);


    if (task_id == 0)
    {
        array_to_sort = malloc(sizeof(int) * num_elements);
        for (int i = 0; i < num_elements; i++)
        {
            //array_to_sort[i] = rand() % 1000;
            array_to_sort[i] = i+1;

        }
        printf("array to sort\n");
        for (int i = 0; i < num_elements; i++)
        {
            printf("%d ",array_to_sort[i]);
        }
        printf("\n");

        size = num_elements/num_tasks;
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        recv_buf = malloc(sizeof(int)*size);
        MPI_Scatter(array_to_sort, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        recv_buf = malloc(sizeof(int) * size);

        MPI_Scatter(NULL, 0, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    // TODO free array_to sort?

    /* END initial scattering*/

    // make a copy of the buffer?
     temp = (int*)malloc(sizeof(int)*size);
     for (int i = 0; i < size; i++)
	 {
         temp[i] = recv_buf[i];
     }
    active_data = temp;


    char* binary_str=to_binary(task_id,3);
    printf("P%d:%s", task_id,binary_str);

    num_elements = size;

    bcast_mask = ((int)pow(2, degree) - 1);

    for (int i = degree-1; i >= 0; i--)
    {
        /* split world to bcast */
        bcast_color = ~bcast_mask & task_id;
        bcast_mask = bcast_mask ^ (int)(pow(2, i));
        MPI_Comm_split(MPI_COMM_WORLD, bcast_color, task_id, &childcomm);
        MPI_Comm_rank(childcomm, &child_id);

        if (child_id == 0)
        {
            pivot = recv_buf[size/2];
            MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm);
        }
        else
        {
            MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm);
        }

        /* split world to send arrays, 2 per comm */
        mask = ((int)pow(2, degree) - 1) ^(int)(pow(2,i));
        color = mask & task_id;
        MPI_Comm_split(MPI_COMM_WORLD, color, task_id, &childcomm);
        MPI_Comm_rank(childcomm, &child_id);

        size_t arr_size_to_send, arr_size_recv;
        if (child_id == 0)
        {
            arr_size_to_send = num_elements;
            MPI_Send(&arr_size_to_send, 1, MPI_INT, 1, 0, childcomm);

            for (int a = 0; a < arr_size_to_send; a++)
            {
                MPI_Send(&active_data[a], 1, MPI_INT, 1, 0, childcomm);

            }


            arr_size_recv = 0;
            MPI_Recv(&arr_size_recv, 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);
            data = malloc(sizeof(int) * arr_size_recv);

            for (int a = 0; a < arr_size_recv; a++)
            {
                MPI_Recv(&data[a], 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);
            }


        }
        else
        {
            arr_size_recv = 0;
            MPI_Recv(&arr_size_recv, 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);
            data = malloc(sizeof(int)*arr_size_recv);
            for (int a = 0; a < arr_size_recv; a++)
            {
                MPI_Recv(&data[a], 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);
            }


            arr_size_to_send = num_elements;
            MPI_Send(&arr_size_to_send, 1, MPI_INT, 0, 0, childcomm);

            for (int a = 0; a < arr_size_to_send; a++)
            {
                MPI_Send(&active_data[a], 1, MPI_INT, 0, 0, childcomm);
            }
        }
        int size_working_arr = arr_size_recv + arr_size_to_send;
        int* working_arr = malloc(sizeof(int)*size_working_arr);
        //num_elements important
        for (int i = 0; i < arr_size_recv; i++)
        {
            working_arr[i] = data[i];
        }
        int start = 0;
        for (int i = arr_size_recv; i < size_working_arr; i++)
        {
            working_arr[i] = active_data[start];
            start++;
        }
        printf("||");
        //split into two array according to pivot
        int* lower = malloc(sizeof(int) * size_working_arr);
        int* upper = malloc(sizeof(int) * size_working_arr);
        lower_count = 0;
        upper_count = 0;
        if (child_id == 0)
        {
            for (int i = 0; i < size_working_arr; i++)
            {
                if (pivot > working_arr[i])
                {
                    lower[lower_count] = working_arr[i];
                    lower_count++;
                }
            }
            /* get the number in the array you keep and track the size */
            num_elements = lower_count;
            active_data = lower;

        }
        else
        {
            for (int i = 0; i < size_working_arr; i++)
            {
                if (pivot <= working_arr[i])
                {
                    upper[upper_count] = working_arr[i];
                    upper_count++;
                }
            }
            num_elements = upper_count;
            active_data = upper;
        }
        if (i == 0)
        {
            printf(" [%d]", pivot);
            for (int i = 0; i < num_elements; i++)
            {
                printf("\t%d ", active_data[i]);
            }
        }
       

    }











	if(task_id == 0)
	{
		printf("\nsorting\n");
		qusort(recv_n,0,15);	
        for (int i = 0; i < 16; i++)
        {
            int data = recv_n[i];
            printf("%d, ", data);
        }
        printf("\n");
	}
  

    MPI_Finalize();
    return 0;
}

