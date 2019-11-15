#include <stdio.h>
#include <stdlib.h>

void print_arr(int** arr, int num)
{
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num; j++)
            printf("%d\t", arr[i][j]);
        printf("\n");
    }
}

void insert_node(int** adj, int num)
{
    for (int i = 0; i <= num; i++) {
        adj[i][num] = 0;
        adj[num][i] = 0;
    }
}

void insert_edge(int** adj, int x, int y, int weight)
{
    adj[x][y] = weight;
    adj[y][x] = weight;
}

int** init_array(int num)
{
    int** adj = (int**)malloc(sizeof(int*) * num);
    for (int i = 0; i < num; i++)
    {
        adj[i] = (int*)malloc(sizeof(int) * num);
    }


    return adj;
}

int** fill_array(int** adj, int num)
{
    for (int i = 0; i < num; i++)
    {
        insert_node(adj, i);
    }
    return adj;
}



int main(int argc, char* argv[])
{
    //Initialize
    int num_ints = 4;

    int** adj = init_array(num_ints);
    //Fill 2d Array
    adj = fill_array(adj, num_ints);
    insert_edge(adj,0,2,1);
    insert_edge(adj,1,2,2);
    insert_edge(adj,3,2,3);

    print_arr(adj, num_ints);




    if (adj)
    {
        for (int i = 0; i < num_ints; i++)
            free(adj[i]);
        free(adj);
    }
    return 0;
}