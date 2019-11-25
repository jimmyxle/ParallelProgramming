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
    start = clock();

    int** adj = init_array(num_ints);
    int** linear_arr = malloc(sizeof(int)* (num_nodes*num_nodes) );
    //Fill 2d Array
    adj = fill_array(adj, num_ints);
    
    clock_t goal = num_nodes*19 + clock();
    while (goal > clock());
        

    

    print_arr(adj, num_ints);

    

    int** dist = floyd(adj,num_ints);

    printf("\nThe new distances are:\n");
    print_arr(dist, num_ints);


    end = clock();

    double elapsed = ((double)(end - start));
    printf("Duration: %0.f ms\n", elapsed / (CLOCKS_PER_SEC / 1000.0));

    return 0;
}
