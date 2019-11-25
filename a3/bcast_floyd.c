#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mpi.h"
#include <time.h>


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

    insert_edge(adj, 0, 1, 3);
    insert_edge(adj, 0, 2, 4);
    insert_edge(adj, 0, 3, 5);
    insert_edge(adj, 0, 5, 6);

    insert_edge(adj, 1, 2, 4);
    insert_edge(adj, 1, 5, 5);
    insert_edge(adj, 1, 6, 2);

    insert_edge(adj, 2, 6, 1);
    insert_edge(adj, 2, 3, 4);
    insert_edge(adj, 2, 7, 3);
    insert_edge(adj, 2, 8, 3);

    insert_edge(adj, 3, 8, 1);
    insert_edge(adj, 3, 9, 3);
    insert_edge(adj, 3, 10, 1);
    
    insert_edge(adj, 4, 3, 7);
    insert_edge(adj, 4, 10, 2);

    insert_edge(adj, 5, 6, 2);
    insert_edge(adj, 5, 11, 8);

    insert_edge(adj, 6, 11, 4);
    insert_edge(adj, 6, 7, 3);
    insert_edge(adj, 6, 8, 6);

    insert_edge(adj, 7, 11, 4);
    insert_edge(adj, 7, 12, 5);
    insert_edge(adj, 7, 13, 1);
    insert_edge(adj, 7, 8, 9);

    insert_edge(adj, 8, 9, 4);
    insert_edge(adj, 8, 14, 4);
    insert_edge(adj, 8, 13, 1);

    insert_edge(adj, 9, 14, 9);
    insert_edge(adj, 9, 15, 9);
    insert_edge(adj, 9, 18, 7);

    insert_edge(adj, 10, 9, 3);
    insert_edge(adj, 10, 15, 1);

    insert_edge(adj, 11, 12, 9);
    insert_edge(adj, 11, 16, 1);

    insert_edge(adj, 12, 13, 2);
    insert_edge(adj, 12, 16, 3);
    insert_edge(adj, 12, 17, 3);

    insert_edge(adj, 13, 17, 1);
    insert_edge(adj, 13, 19, 2);

    insert_edge(adj, 14, 19, 9);
    insert_edge(adj, 14, 18, 1);
    insert_edge(adj, 14, 15, 3);

    insert_edge(adj, 15, 18, 5);

    insert_edge(adj, 16, 17, 2);

    insert_edge(adj, 17, 19, 1);

    insert_edge(adj, 18, 19, 4);
    
	return adj;
}

int main(int argc, char* argv[])
{
	//Declare all variables 
	int num_task, task_id, num_nodes , num_elements,scatter_size, *value, row_id, col_id;
	int** adj, *linear_arr, *recv_buf;
    int d_i_k, d_k_j, i, j, k;
    MPI_Comm colcomm, rowcomm;
    clock_t start, end;

    //Initialize MPI
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &num_task);
	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

    /*
        I figured out that for d_ij, i and k can be calculated as: 
        i = rank proc % number of nodes ie: 15/4 = 3 
        j = rank proc / number of nodes ie: 15%4 = 3 

        Split the world into columns and rows
    */
    if (argc != 2)
        printf("not enough args");
    else
        num_nodes = atoi(argv[1]);

    i = task_id % num_nodes;
    j = task_id / num_nodes;
    MPI_Comm_split(MPI_COMM_WORLD, i, task_id, &colcomm);
    MPI_Comm_rank(colcomm, &col_id);

    MPI_Comm_split(MPI_COMM_WORLD, j, task_id, &rowcomm);
    MPI_Comm_rank(rowcomm, &row_id);





	num_elements = num_nodes*num_nodes;
    //Scatter size is how many elements each processor holds
    scatter_size = 1;

    linear_arr = (int*)malloc(sizeof(int) * num_task);
    recv_buf = (int*)malloc(sizeof(int) * scatter_size);
    value = (int*)malloc(sizeof(int) * scatter_size);



	if (task_id == 0)
	{
        //Initialize the matrix
		adj = init_array(num_nodes);
		adj = fill_array(adj, num_nodes);
		
		print_arr(adj, num_nodes);

        //Needed the 2d array as a 1d array
		for (int s = 0; s < num_nodes; s++)
		{
			for(int t = 0; t < num_nodes; t++)
			{
				linear_arr[num_nodes*s+t] = adj[s][t];
			}
		}
        printf("\n");

	}

    //Each processor has one element, value
    MPI_Scatter(linear_arr, scatter_size, MPI_INT, &recv_buf, scatter_size, MPI_INT, 0, MPI_COMM_WORLD);
    value = recv_buf;
    if (task_id == 0)
        start = clock();

    k = 1;
    while (k < num_nodes)
    {
        /*
            both d_i_k and d_k_j are used for the djikstra calculation 
            to get the minimum cost
        */
        d_i_k = INT_MAX;
        d_k_j = INT_MAX;

        //MPI_Barrier(MPI_COMM_WORLD);
        /*
            if k == j, then you share your data with the column
        */
        if ( k == j )
        {
            recv_buf = value;
            MPI_Bcast(&recv_buf, 1, MPI_INT, k, colcomm);
        }
        else
        {
            MPI_Bcast(&recv_buf, 1, MPI_INT, k, colcomm);
            d_i_k = recv_buf;
        }
        /*
            if k == i, you share the data with the row
        */
        if (k == i)
        {
            recv_buf = value;
            MPI_Bcast(&recv_buf, 1, MPI_INT, k, rowcomm);
        }
        else
        {
            MPI_Bcast(&recv_buf, 1, MPI_INT, k, rowcomm);
            d_k_j = recv_buf;
        }
        /*
            check if the val or d_i_k + d_j_k is the minimum
        */
        if (d_i_k < INT_MAX && d_k_j < INT_MAX && (value > (d_i_k + d_k_j)) )
        {
                value = (d_i_k + d_k_j);
        }
        k++;
    }  
    if (task_id == 0)
        end = clock();

    MPI_Gather(&value, 1, MPI_INT, linear_arr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (task_id == 0)
    {    

        for (int i = 0; i < num_task; i++)
        {
            if (i % num_nodes == 0)
                printf("\n");
            if (linear_arr[i] == INT_MAX)
                printf("INF\t" );
            else
                printf("%d \t", linear_arr[i]);
        }
        printf("\n");

        double elapsed = ((double)(end - start));
        printf("Duration: %0.f ms\n", elapsed / (CLOCKS_PER_SEC/1000.0));
    }


    

	MPI_Finalize();
	return 0;
}
