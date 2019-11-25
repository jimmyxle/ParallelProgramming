#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mpi.h"

#define INF (INT_MAX)

void print_arr(int** arr, int num)
{
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
    for (int i = 0; i < num; i++)
        insert_node(adj, i);
    return adj;
}

int main(int argc, char* argv[])
{
    //Declare all variables 
    int num_task, task_id, num_nodes = 4, num_elements = 0, scatter_size, * value, row_id, col_id;
    int** adj, * linear_arr, * recv_buf, *local_arr, *outgoing;
    int d_i_k, d_k_j, i, j, k;
    MPI_Comm colcomm, rowcomm;

    //Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &num_task);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

    



    if (argc != 2)
        printf("not enough args");
    else
        num_elements = atoi(argv[1]);

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
        insert_edge(adj, 0, 3, 10);
        insert_edge(adj, 0, 1, 5);
        insert_edge(adj, 1, 2, 3);
        insert_edge(adj, 2, 3, 1);
        print_arr(adj, num_nodes);

        //Needed the 2d array as a 1d array
        for (int s = 0; s < num_nodes; s++)
        {
            for (int t = 0; t < num_nodes; t++)
            {
                //linear_arr[num_nodes * s + t] = adj[s][t];
                linear_arr[num_nodes * s + t] = num_nodes*s+t;
            }
        }
        for (int r = 0; r < num_elements; r++)
        {
            if (r % num_nodes == 0)
            {
                printf("\n");
            }
            printf("%d\t ", linear_arr[r]);

        }
        printf("\n%d num elements \n", num_elements);

    }
    /*
        each processor, send info, do work
        each processor does work at k
    */

    k = task_id;
    int* incoming_data;
    int outgoing_data = 0;
    int* data_send;
    i = task_id % num_nodes;
    j = task_id / num_nodes;

    int* index;

    if (task_id == 0)
    {
        //proc0 starts the chain
        //do work

        //send work
        for (int r = 0; r < num_elements; r++)
        {
            //send the index
            data_send = &r;
            MPI_Send(data_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

            data_send = &linear_arr[r];
            MPI_Send(data_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        
        for (int r = 0; r < num_elements; r++)
        {
            MPI_Recv(recv_buf, scatter_size, MPI_INT, (task_id - 1), 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            index = recv_buf;

            MPI_Recv(recv_buf, scatter_size, MPI_INT, (task_id-1), 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            local_arr[*index] = *recv_buf;

            //printf("[%d]: %d at index %d received from [%d]\n", task_id, *index, local_arr[*index], task_id - 1);


            // do work if you get everything

            //if you do work, put in outgoing arr
           

            //check if the val, plus its dependents have actual values. Earlier these were set to -1
            if (local_arr[r] >= 0 && local_arr[k + num_nodes * j] >= 0 && local_arr[num_nodes * k + i] >= 0)
            {
                d_i_k = local_arr[k + num_nodes * j];
                d_k_j = local_arr[num_nodes * k + i];
                if (d_i_k < INT_MAX && d_k_j < INT_MAX)
                { 
                if(value > (d_i_k + d_k_j))
                {
                    value = (d_i_k + d_k_j);
                    printf("index [%d] : %d + %d = %d\n", r, d_i_k, d_k_j, value);
                    local_arr[r] = value;
                }
                else
                {
                    value = local_arr[r];
                    printf("index [%d] : value is now: %d\n", r, value);

                }

                //send this data to next except if you're the last processor
                if (task_id != num_task - 1)
                {
                    // send index
                    data_send = &r;
                    MPI_Send(data_send, 1, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD);

                    //send data
                    data_send = local_arr[r];
                    MPI_Send(data_send, scatter_size, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD);

                    printf("[%d]: %d sent to [%d]\n", task_id, data_send, task_id + 1);
                    
                }
                outgoing[r] = 1;
                }
                
            } 
        }
        /* This is for any elements not sent yet */
        for (int r = 0; r < num_elements; r++)
        {
            if (outgoing[r] != 1)
            {
                d_i_k = local_arr[k + num_nodes * j];
                d_k_j = local_arr[num_nodes * k + i];
                if (d_i_k < INT_MAX && d_k_j < INT_MAX && (value > (d_i_k + d_k_j)))
                {
                    value = (d_i_k + d_k_j);
                    printf("index [%d] : %d + %d = %d\n", r, d_i_k, d_k_j, value);
                    local_arr[r] = value;
                }
                else
                {
                    value = local_arr[r];
                    printf("index [%d] : value is now: %d\n", r, value);

                }

                if (task_id != num_task - 1)
                {
                    value = local_arr[r];
                    printf("[%d]: %d sent to [%d]\n", task_id, outgoing_data, task_id + 1);
                    MPI_Send(&value, scatter_size, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD);
                }
                outgoing[r] = 1;
            }
        }


        //if not 0, waits for work
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
