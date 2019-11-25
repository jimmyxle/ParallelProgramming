#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>


#define INF (INT_MAX)

void print_arr(int** arr, int num)
{
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num; j++)
        {
            if( arr[i][j] == INF)
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

int** floyd(int** adj, int num)
{
    int** dist = init_array(num);
    //Copy array to manipulate later
    for (int i = 0; i < num; i++)
    {
        dist[i] = (int*)malloc(sizeof(int*)*num);
        memcpy((dist[i]), (adj[i]), sizeof(int) * num);
    }
    for (int k = 0; k < num; k++)
    {
        for (int i = 0; i < num; i++)
        {
            for (int j = 0; j < num; j++)
                if (dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j]; 
        }
    }
    return dist;
}


int main(int argc, char* argv[])
{
    //Initialize
    int num_nodes = 0; //should be number of total elements 

    if (argc != 2)
        printf("not enough args");
    else
        num_nodes = atoi(argv[1]);

    int num_ints = num_nodes;

    clock_t start, end;

    int** adj = init_array(num_ints);
    //Fill 2d Array
    adj = fill_array(adj, num_ints);
    insert_edge(adj,0,3,10);
    insert_edge(adj,0,1,5);
    insert_edge(adj,1,2,3);
    insert_edge(adj,2,3,1);

    print_arr(adj, num_ints);
    start = clock();

    int** dist = floyd(adj,num_ints);
    end = clock();

    printf("\nThe new distances are:\n");
    print_arr(dist, num_ints);

    double elapsed = ((double)(end - start));
    printf("Duration: %0.f s\n", elapsed/ CLOCKS_PER_SEC);

    return 0;
}
