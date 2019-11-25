#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mpi.h"
#include <time.h>

#define INF (99)

void print_arr(int** arr, int num)
{
    printf("\n\nPrinting original matrix\n\n");
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num; j++)
        {
            if (arr[i][j] == INF)
                printf("INF\t");
            else
                printf("%d\t", arr[i][j]);
        }
        printf("\n");
    }
}

void insert_node(int** adj, int num)
{
    for (int i = 0; i <= num; i++) {
        adj[i][num] = INF;
        adj[num][i] = INF;
    }
    for (int i = 0; i <= num; i++) {
        adj[i][i] = 0;
    }
}

void insert_edge(int** adj, int x, int y, int weight)
{
    adj[x][y] = weight;
}

int** init_array(int num)
{
    int** adj = (int**)malloc(sizeof(int*) * num);
    for (int i = 0; i < num; i++)
        adj[i] = (int*)malloc(sizeof(int) * num);
    return adj;
}

int** fill_array(int** adj, int num)
{
    time_t t;
    srand((unsigned)time(&t));
    for (int i = 0; i < num; i++)
        insert_node(adj, i);
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num; j++)
        {
            int temp1 = rand() % (num);
            int temp2 = rand() % (num);
            if (temp1 != temp2)
            {
                insert_edge(adj, temp1, temp2, rand() % 10);
            }
        }
    }
    return adj;
}

int main(int argc, char* argv[])
{
    //Declare all variables 
    int num_task, task_id, num_nodes, num_elements, scatter_size, * value, row_id, col_id;
    int** adj, * linear_arr, * recv_buf, *local_arr, *outgoing;
    int d_i_k, d_k_j, i, j, k;
    MPI_Comm colcomm, rowcomm;
    MPI_Request request;

    //Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &num_task);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

    



    if (argc != 2)
        printf("not enough args");
    else
        num_nodes = atoi(argv[1]);

    num_elements = num_nodes*num_nodes;

    //Scatter size is how many elements each processor holds
    scatter_size = 1;

    linear_arr = (int*)malloc(sizeof(int) * num_task);
    recv_buf = (int*)malloc(sizeof(int) * scatter_size);
    value = (int*)malloc(sizeof(int) * scatter_size);
    local_arr = (int*)malloc(sizeof(int) * num_task);
    outgoing = (int*)malloc(sizeof(int) * num_elements);
    //set local_arr to be filled with negatives. Useful for when we start receivng data, we know which elements are 
    //available if they're positive.
    for (int r = 0; r < num_elements; r++)
    {
        local_arr[r] = -1;
        outgoing[r] = 0;
    }


    if (task_id == 0)
    {
        //Initialize the matrix
        adj = init_array(num_nodes);
        adj = fill_array(adj, num_nodes);
  
        print_arr(adj, num_nodes);

        //Needed the 2d array as a 1d array
        for (int s = 0; s < num_nodes; s++)
        {
            for (int t = 0; t < num_nodes; t++)
            {   
            int* data = adj[s][t];

                //linear_arr[num_nodes * s + t] = data;
                linear_arr[num_nodes * s + t] = num_nodes*s+t;
            }
        }
        printf("\nOriginal\n");
        for (int r = 0; r < num_elements; r++)
        {
            if (r % num_nodes == 0)
            {
                printf("\n");
            }
            printf("%d\t ", linear_arr[r]);

        }
        printf("\n%d num elements \n\n", num_elements);

    }
    /*
        each processor, send info, do work
        each processor does work at k
    */

    k = task_id;
    int* incoming_data;
    int outgoing_data = 0;
    int* data_send;
    

    int* index;
    int target_id = 1;


    if (task_id == 0)
    {
        
        for (int r = 0; r < num_elements; r++)
        {
            //send the index
            data_send = r;

            MPI_Isend(&data_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
            printf("\t\t\t[%d] %d", task_id, data_send);

            data_send = &linear_arr[r];
            MPI_Isend(data_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
            printf("\t[%d] %d\n", task_id, *data_send);

        }
    }
    else
    {
        int r =0;
        int* index = 0;
        

        while (r < num_elements)
        {
            //receive from previous proc
            MPI_Irecv(recv_buf, scatter_size, MPI_INT, (task_id - 1), 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
            index = recv_buf;
            /**/
            if (task_id == target_id)
                printf("index recv %d ",*index);

            MPI_Irecv(recv_buf, scatter_size, MPI_INT, (task_id - 1), 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
            local_arr[*index] = *recv_buf;

            if (task_id == target_id)
                printf("val recv %d \n", local_arr[*index]);


            //do work if possible
            

            for (int s = 0; s < num_elements; s++)
            {
                i = s % num_nodes;
                j = s / num_nodes;
                d_i_k = local_arr[k + num_nodes * j];
                d_k_j = local_arr[num_nodes * k + i];

                if (outgoing[s] != 1)
                {
                    //printf("[%d]: %d %d\n",task_id, d_i_k, d_k_j);

                    if (d_i_k < INT_MAX && d_k_j < INT_MAX)
                    {
                        if (local_arr[s] >= 0 && d_i_k >= 0 && d_k_j >= 0)
                        {
                            if (local_arr[s] > (d_i_k + d_k_j))
                            {
                          
                                local_arr[s] = (d_i_k + d_k_j);
                            /**/
                                if (task_id == target_id)
                                    printf("LARGER index [%d] : %d + %d = %d, < %d\n", s, d_i_k, d_k_j,d_i_k+d_k_j ,local_arr[s]);

                                local_arr[s] = value;
                            }
                            else
                            {

                                value = local_arr[s];
                            /**/
                                if (task_id == target_id)
                                    printf("SMALLER index [%d] : %d + %d = %d, > %d\n", s, d_i_k, d_k_j, d_i_k + d_k_j, local_arr[s]);

                            }
                            outgoing[s] = 1;

                            if (task_id < num_task - 1)
                            {
                                data_send = s;
                                MPI_Isend(&data_send, 1, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD, &request);

                                data_send = &local_arr[s];
                                MPI_Isend(data_send, scatter_size, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD, &request);
                                if (task_id == target_id)
                                    printf("data sent %d, %d \n", s, local_arr[s]);
                            }
                            r++;

                        }
                    }
                }
            }

        }
       

        
        //if not 0, waits for work
        if (task_id == target_id)
        {
            printf("[%d]\n", task_id);

            for (int r = 0; r < num_elements; r++)
            {
                if (r % num_nodes == 0)
                {
                    printf("\n");
                }
                printf("%d\t ", local_arr[r]);

            }

        }
        
        

    }

   /* printf("[%d]\n", task_id);

    for (int r = 0; r < num_elements; r++)
    {
        if (r % num_nodes == 0)
        {
            printf("\n");
        }
        printf("%d\t ", local_arr[r]);

    }*/

    MPI_Barrier(MPI_COMM_WORLD);



    //if (task_id == 0)
    //{
    //    for (int i = 0; i < num_task; i++)
    //    {
    //        if (i % 4 == 0)
    //            printf("\n");
    //        if (linear_arr[i] == INT_MAX)
    //            printf("INF\t");
    //        else
    //            printf("%d \t", linear_arr[i]);
    //    }
    //    printf("\n");

    //}
    MPI_Finalize();
    return 0;
}
