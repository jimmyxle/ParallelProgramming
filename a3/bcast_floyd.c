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
int** bcast_floyd(int num_nodes, int num_proc, int task_id)
{
    
	



	/*
	SYNC

	k == 1
	if i == 0 and j == k, share with all in same column. Where i == 0
	if i == 1 and j == k, share with all in same column. Where i == 1
	if i == 2 and j == k, share with all in same column. Where i == 2

	if you are i,k SHARE

	if i == k, j == 0, share with all in the same row, where j == 0
	if i == k, j == 1, share with all in the same row, where j == 1
	if i == k, j == 2, share with all in the same row, where j == 2
	
	if you are k,j SHARE
	
	Do computation

	SYNC 

	k == 2

	if j == k, share with everyone with i == 0 (max 2 times)
	if j == k, share with everyone with i == 1
	if j == k, share with everyone with i == 2

	if i == k, share with all with j == 0
	if i == k, share with all with j == 1
	if i == k, share with all with j == 2

	Do computation

	when done do gatherv by rank of processor



	notes:
	everyone does their own calculation 
	SOME send twice
	everyone recv 2

	*/


	return NULL;
}

int main(int argc, char* argv[])
{
	//Initialize
	int num_task, task_id, num_nodes = 4, num_elements,scatter_size, *value, row_id, col_id;
	int** adj,**dist, *linear_arr, *recv_buf;

	MPI_Init(NULL, NULL);
	MPI_Comm colcomm, rowcomm;
	MPI_Comm_size(MPI_COMM_WORLD, &num_task);
	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    int i = task_id % num_nodes;
    int j = task_id / num_nodes;
    MPI_Comm_split(MPI_COMM_WORLD, i, task_id, &colcomm);
    MPI_Comm_rank(colcomm, &col_id);

    MPI_Comm_split(MPI_COMM_WORLD, j, task_id, &rowcomm);
    MPI_Comm_rank(rowcomm, &row_id);



	if (argc != 2)
		printf("not enough args");
	else
		num_elements = atoi(argv[1]);
	
//	printf("task_id[%d]:\n",task_id);

    linear_arr = (int*)malloc(sizeof(int) * num_task);
    //use linear_arr for scatter
    scatter_size = 1;
    recv_buf = (int*)malloc(sizeof(int) * scatter_size);
    value = (int*)malloc(sizeof(int) * scatter_size);


	if (task_id == 0)
	{
		adj = init_array(num_nodes);
		//Fill 2d Array
		adj = fill_array(adj, num_nodes);
		insert_edge(adj, 0, 3, 10);
		insert_edge(adj, 0, 1, 5);
		insert_edge(adj, 1, 2, 3);
		insert_edge(adj, 2, 3, 1);
		print_arr(adj, num_nodes);
		//turn 2d array into 1d array

		for (int s = 0; s < num_nodes; s++)
		{
			for(int t = 0; t < num_nodes; t++)
			{
				//linear_arr[num_nodes*s+t] = adj[s][t];
				linear_arr[num_nodes*s+t] = num_nodes * s + t;
			}
			 
		}
        /*for (int i = 0; i < num_nodes*num_nodes; i++)
        {
            printf("[%d]%d\t(%d)\t ", i,linear_arr[i], sizeof(linear_arr[i]));
        }*/
        printf("\n");
	}
    MPI_Scatter(linear_arr, scatter_size, MPI_INT, &recv_buf, scatter_size, MPI_INT, 0, MPI_COMM_WORLD);
    value = recv_buf;

    printf("%d has... \t", task_id);
    printf("%d\t\n", value);

    /*
    each process has their own number
    so scatter by rank of processor
    */
    int k = 0;
    //int i = rank proc / number of nodes ie: 15/4 = 3 
    //int j = rank proc % number of nodes ie: 15%4 = 3 
    int* d_i_k = INT_MAX, d_k_j = INT_MAX;

    while (k < 2)
    {
        if (k == 0)
        {
            //value is k
            //start k at 1 instead
            //printf("[%d] HAS %d when k == 0\n", task_id, value);

        }
        else
        {
            MPI_Barrier(MPI_COMM_WORLD);
            if ( k == j )
            {
                int val = value;
                //printf("[%d] shared %d with the column\n", task_id, val);
                recv_buf = value;
                /*MPI_Bcast(&recv_buf, 1, MPI_INT, k, colcomm);
                printf("[%d] recv %d \n", task_id, val);*/

            }
            else
            {
                //printf("d_i_k before: %d\n", d_i_k);
                MPI_Bcast(&recv_buf, 1, MPI_INT, k, colcomm);
                d_i_k = recv_buf;
                int val = value;
         /*       printf("[%d] has val %d and ", task_id, val);
                printf("d_i_k after: %d\n", d_i_k);*/

            }

            //if (k == j)
            //{
            //    MPI_Bcast(value, 1, MPI_INT, num_elements * k, colcomm);

            //}
            //else
            //{
            //    MPI_Bcast(d_k_j, 1, MPI_INT, num_elements * k, colcomm);

            //}
            int min = INT_MAX;
            if (d_i_k < INT_MAX && d_k_j < INT_MAX)
            {
                if (value < (d_i_k + d_k_j))
                {
                    min = value;

                }
                else
                {
                    min = (d_i_k + d_k_j);
                }
            }
            else
            {
                min = value;
            }

 
            printf("[%d] min is %d", task_id, min);

            value = min;
        }

        
        k++;

    }
	/*
	if (task_id == 0)
	{


		dist = bcast_floyd(adj, num_ints);


		printf("\nThe new distances are:\n");
		print_arr(dist, num_ints);

	}*/
	
	MPI_Finalize();
	return 0;
}
