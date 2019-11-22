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
int** bcast_floyd(int** adj, int num)
{

	/*
	each process has their own number 
	so scatter by rank of processor

	
	*/
	int k = 0;
	//int i = rank proc / number of nodes ie: 15/4 = 3 
	//int j = rank proc % number of nodes ie: 15%4 = 3 
	// processor 15 is at 3,3
	//while k < num -1


	/*


	for n = 3, 9 processes 0 <= k <= 2
	k == 0, everyone does their own 

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


	return adj;
}

int main(int argc, char* argv[])
{
	//Initialize
	int num_task, task_id, num_ints = 4, num_elements,scatter_size;
	int** adj,**dist, *linear_arr, *recv_buf;

	MPI_Init(NULL, NULL);
	MPI_Comm childcomm;
	MPI_Comm_size(MPI_COMM_WORLD, &num_task);
	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	MPI_Status* status;
	if (argc != 2)
		printf("not enough args");
	else
		num_elements = atoi(argv[1]);
	
//	printf("task_id[%d]:\n",task_id);

	int num;
	if (task_id == 0)
	{
		adj = init_array(num_ints);
		//Fill 2d Array
		adj = fill_array(adj, num_ints);
		insert_edge(adj, 0, 3, 10);
		insert_edge(adj, 0, 1, 5);
		insert_edge(adj, 1, 2, 3);
		insert_edge(adj, 2, 3, 1);
		print_arr(adj, num_ints);
		//turn 2d array into 1d array
		linear_arr = (int*)malloc(sizeof(int)*num_task);
		for (int i = 0; i < num_ints; i++)
		{
			for(int j = 0; j < num_ints; j++)
			{
				linear_arr[num_ints*i+j] = adj[i][j];
			}
			 
		}
		//use linear_arr for scatter
		scatter_size = 1;
		//MPI_Send(&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		recv_buf = malloc(sizeof(int)*scatter_size);		
		MPI_Scatter(&linear_arr, scatter_size, MPI_INT, recv_buf, scatter_size, MPI_INT, 0, MPI_COMM_WORLD);
		printf("%d scattered data\n", task_id);			
	}
	else
	{
			
		printf("%d receiving... \t",task_id);
		scatter_size = 1;
		recv_buf = malloc(sizeof(int)*scatter_size);
		//MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Scatter(&linear_arr, scatter_size, MPI_INT, recv_buf, scatter_size, MPI_INT, 0, MPI_COMM_WORLD);
		int data = recv_buf[0];	
		printf("%d\t\n", data);
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
