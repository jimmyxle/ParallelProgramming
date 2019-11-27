#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mpi.h"
#include <time.h>

#define INF (INT_MAX)

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
                insert_edge(adj, temp1, temp2, rand() % (20));
            }
        }
    }
    return adj;
}

int main(int argc, char* argv[])
{
    //Declare all variables 
    int num_task, task_id, num_nodes, num_elements, scatter_size, * value;
    int** adj, * linear_arr, * recv_buf, *local_arr, *outgoing;
    int d_i_k, d_k_j, i, j, k;
    MPI_Request request;
	clock_t start, end;

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

    linear_arr = (int*)malloc(sizeof(int) * num_elements);
    recv_buf = (int*)malloc(sizeof(int) * scatter_size);
    value = (int*)malloc(sizeof(int) * scatter_size);
    local_arr = (int*)malloc(sizeof(int) * num_elements);
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
  
        //print_arr(adj, num_nodes);

        //Needed the 2d array as a 1d array
        for (int s = 0; s < num_nodes; s++)
        {
            for (int t = 0; t < num_nodes; t++)
            {   
                linear_arr[num_nodes * s + t] = adj[s][t];
                //linear_arr[num_nodes * s + t] = rand() % (num_nodes);
            }
        }
        printf("\nOriginal\n");
		for (int r = 0; r < num_elements; r++)
		{
			if (r % num_nodes == 0)
			{
				printf("\n");
			}
			if (linear_arr[r] == INF)
				printf("INF\t");
			else
				printf(" %d\t ", linear_arr[r]);

		}
        printf("\n");
	
    }
    /*
        each processor, send info, do work
        each processor does work at k
    */

    k = task_id;
    int outgoing_data = 0;
    int* data_send;
    

    int target_id = num_task-1;
	start = clock();

    if (num_task == 1)
    {
        for (int r = 0; r < num_elements; r++)
        {
            //send the index
            data_send = &r;

            //printf("\t\t\t[%d] %d", task_id, data_send);

            data_send = &linear_arr[r];
            //printf("\t[%d] %d\n", task_id, *data_send);

        }
    }

    else if (task_id == 0 && num_task > 1)
    {
        
        for (int r = 0; r < num_elements; r++)
        {
            //send the index
            data_send = &r;

            MPI_Isend(data_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
            //printf("\t\t\t[%d] %d", task_id, data_send);

            data_send = &linear_arr[r];
            MPI_Isend(data_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);


            //printf("\t[%d] %d\n", task_id, *data_send);

        }
    }
    else
    {
        int r = 0;
        int recv_count = 0;
        int send_count = 0;


        while (r < num_elements)
        {
            if (recv_count < num_elements)
            {
                
                //receive from previous proc
                MPI_Irecv(recv_buf, scatter_size, MPI_INT, (task_id - 1), 0, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, MPI_STATUSES_IGNORE);
                int index = *recv_buf;
                            
                /**/
                /*if (task_id == target_id)
                    printf("index recv %d :",index);*/

                MPI_Irecv(recv_buf, scatter_size, MPI_INT, (task_id - 1), 0, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, MPI_STATUSES_IGNORE);



                /*if (task_id == target_id)
                    printf("BEFORE: %d \t", local_arr[index]);*/
                local_arr[index] = *recv_buf;
                recv_count++;

               /* if (task_id == target_id)
                    printf("AFTER: %d \n", local_arr[index]);*/
            }
            
			for (int s = 0; s < num_elements; s++)
			{
				i = s % num_nodes;
				j = s / num_nodes;
				d_i_k = local_arr[k + num_nodes * j];
				d_k_j = local_arr[num_nodes * k + i];

				if (outgoing[s] != 1)
				{

					//all values initialized to -1, check to see if they have been changed
					if (d_i_k >= 0 && d_k_j >= 0 && local_arr[s] >= 0)
					{

						if (d_i_k < INT_MAX && d_k_j < INT_MAX && (local_arr[s] > (d_i_k + d_k_j)))
						{

							if (local_arr[s] > (d_i_k + d_k_j))
							{

								local_arr[s] = (d_i_k + d_k_j);
								/**/
						/*		if (task_id == target_id)
									printf("LARGER index [%d] : %d + %d = %d, < %d\n", s, d_i_k, d_k_j, d_i_k + d_k_j, local_arr[s]);*/
							}
						}
						else
						{

							value = &local_arr[s];
							/**/
							/*if (task_id == target_id)
								printf("SMALLER index [%d] : %d + %d = %d, > %d\n", s, d_i_k, d_k_j, d_i_k + d_k_j, local_arr[s]);*/

						}

						outgoing[s] = 1;
						/*if (task_id == target_id)
							printf("data sent %d, %d \n", s, local_arr[s]);*/

						if (task_id < num_task - 1)
						{
							data_send = &s;

							MPI_Isend(data_send, 1, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD, &request);
							data_send = &local_arr[s];

							MPI_Isend(data_send, scatter_size, MPI_INT, (task_id + 1), 0, MPI_COMM_WORLD, &request);

							if (task_id == target_id)
								printf("data sent %d, %d \n", s, local_arr[s]);
						}
						send_count++;


						
					}
				}
			}


			if (send_count == num_elements)
			{
				r = num_elements+1;
			}
            


            r++;

        }
       

    }

	if (task_id == (num_task - 1))
	{
		end = clock();

		printf("[%d] OUTPUT \n", task_id);

		for (int r = 0; r < num_elements; r++)
		{
			if (r % num_nodes == 0)
			{
				printf("\n");
			}
			if (local_arr[r] == INF)
				printf("INF\t");
			else
				printf(" %d\t ", local_arr[r]);

		}
		printf("\n");


		double elapsed = ((double)(end - start));
		printf("Duration: %0.f ms\n", elapsed / (CLOCKS_PER_SEC / 1000.0));
	}
 
    MPI_Finalize();
    return 0;
}
