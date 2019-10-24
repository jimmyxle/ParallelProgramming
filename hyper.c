/* jimmy */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

        if (arr[j] > 0)
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
    
    int num_tasks, global_id, task_id,child_id, size, degree = 3, pivot, color, color_degree_1;
    int recv_data[10], upper[10], lower[10];
    int* active_data; 
    int *temp;
    int empty_data[] = {0,0,0,0};
    int lower_count, upper_count;

    MPI_Init(&argc, &argv);

    MPI_Comm childcomm, childcomm_degree_1, childcomm_degree_2;

    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_rank(MPI_COMM_WORLD, &global_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    /*Scatter parts of array to each process*/
    if (task_id == 0)
    {
        int n[16] = { 11,85,21,31,41,51,61,71,81,91,101,111,121,131,141,151 };
        size = (sizeof(n)/sizeof(int))/num_tasks;
   
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(n, size, MPI_INT, &recv_data, size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        int n;
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(&n, 0, MPI_INT, &recv_data, size, MPI_INT, 0, MPI_COMM_WORLD);

    }
    /* END initial scattering*/
     temp = (int*)malloc(sizeof(int)*size);
     for (int i = 0; i < size; i++)
     {
         temp[i] = recv_data[i];
     }
    active_data = temp;
    
    /*
        figure out binary
        rank 0 bcast pivot
    */
    int max = 3;
    for (int d = 0; d < max; d++)
    {
        


        printf("\t[task id: %d] \n", global_id);
        if ( d == 0 )
        {

            //comm split
            if (global_id == 0 || global_id == 4)
            {
                color = 0;
            }
            else if (global_id == 1 || global_id == 5)
            {
                color = 1;
            }
            else if (global_id == 2 || global_id == 6)
            {
                color = 2;
            }
            else
            {
                /* 3 & 7 */
                color = 3;
            }

            /* bcast pivot */
            if (task_id == 0)
            {
                int pivot_index = size/ 2;
                pivot = active_data[pivot_index];
                MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);

            }
            else
            {

                pivot = 0;
                MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);

            }
            printf("pivot data is: %d", pivot);

        }
        else if (d == 1)
        {

            if (global_id == 0 || global_id == 2)
            {
                color = 0;
                color_degree_1 = 0;
            }
            else if (global_id == 1 || global_id == 3)
            {
                color = 1;
                color_degree_1 = 0;

            }
            else if (global_id == 4 || global_id == 6)
            {
                color = 2;
                color_degree_1 = 1;

            }
            else
            {
                /* 5 & 7 */
                color = 3;
                color_degree_1 = 1;

            }
            /* split comms 0 2 1 3, 4 6 5 7*/

           MPI_Comm_split(MPI_COMM_WORLD, color_degree_1, task_id, &childcomm_degree_1);
           MPI_Comm_rank(childcomm_degree_1, &task_id);

           /* bcast pivot */
           if (task_id == 0)
           {
               int pivot_index = size / 2;
               pivot = active_data[pivot_index];
              // printf("\t%d task in degree 1 is sending %d\n", task_id, pivot);
               MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm_degree_1);

           }
           else
           {

               pivot = 0;
               MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm_degree_1);
              //printf("\t%d task in degree 1 is recving %d\n", task_id, pivot);

           }

        }
        else if (d == 2)
        {
            
            if (global_id == 0 || global_id == 1)
            {
                color = 0;
            }
            else if (global_id == 2 || global_id == 3)
            {
                color = 1;

            }
            else if (global_id == 4 || global_id == 5)
            {
                color = 2;

            }
            else
            {
                /* 6 & 7 */
                color = 3;

            }
            /* split comms 0 1, 2 3 , 4 5, 6 7*/

            MPI_Comm_split(MPI_COMM_WORLD, color, task_id, &childcomm_degree_2);
            MPI_Comm_rank(childcomm_degree_2, &task_id);

            /* bcast pivot */
            if (task_id == 0)
            {
                int pivot_index = size / 2;
                pivot = active_data[pivot_index];
                if (pivot < 0)
                {
                    pivot = 0;
                }
                printf("\t%d task in degree 2 is sending %d\n", task_id, pivot);
                MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm_degree_2);

            }
            else
            {

                pivot = 0;
                MPI_Bcast(&pivot, 1, MPI_INT, 0, childcomm_degree_2);
                printf("\t%d task in degree 2 is recving %d\n", task_id, pivot);

            }

        }


        
        MPI_Comm_split(MPI_COMM_WORLD, color, task_id, &childcomm);
        MPI_Comm_rank(childcomm, &child_id);
        
        
        /*
            Split the comm between processor partners
            
        */
        int data[4];
        int active_size, active_response;

        if (child_id == 0)
        {
            active_size = get_size(active_data);
            MPI_Send(&active_size, 1, MPI_INT, 1, 0, childcomm);

            for (int a = 0; a < active_size; a++)
            {
                MPI_Send(&active_data[a], 1, MPI_INT, 1, 0, childcomm);

            }
            //MPI_Send(&active_data[0], 1, MPI_INT, 1, 0, childcomm);
           // MPI_Send(&active_data[1], 1, MPI_INT, 1, 0, childcomm);
            
            active_response = 0;
            MPI_Recv(&active_response, 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);

            for (int a = 0; a < active_response; a++)
            {
                MPI_Recv(&data[a], 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);
            }
           // MPI_Recv(&data[0], 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);
            //MPI_Recv(&data[1], 1, MPI_INT, 1, 0, childcomm, MPI_STATUS_IGNORE);

        }
        else
        {   
            active_response = 0;
            MPI_Recv(&active_response, 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);

            for (int a = 0; a < active_response; a++)
            {
                MPI_Recv(&data[a], 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);
            }
            //MPI_Recv(&data[1], 1, MPI_INT, 0, 0, childcomm, MPI_STATUS_IGNORE);

            active_size = get_size(active_data);
            MPI_Send(&active_size, 1, MPI_INT, 0, 0, childcomm);

            for (int a = 0; a < active_size; a++)
            {
                MPI_Send(&active_data[a], 1, MPI_INT, 0, 0, childcomm);

            }
            //MPI_Send(&active_data[0], 1, MPI_INT, 0, 0, childcomm);
            //MPI_Send(&active_data[1], 1, MPI_INT, 0, 0, childcomm);

        }

        int working_arr[10 ];
        int temp_size = get_size(data) + get_size(active_data);
        printf("temp_size: %d\n", temp_size);

        int arr_count = 0;
        for (int i = 0; i < get_size(data); i++)
        {
            working_arr[arr_count]=data[i];
            arr_count++;
        }
        for (int i = 0; i < get_size(active_data); i++)
        {
            working_arr[arr_count]= active_data[i];
            arr_count++;

        }



        //if (data[0] < 0 )
        //{
        //    working_arr[0] = active_data[0];
        //    working_arr[1] = active_data[1];
        //}
        //if (data[0] > 0 && data[0] < 1)
        //{
        //    working_arr[0] = data[0];
        //    working_arr[1] = active_data[0];
        //    working_arr[2] = active_data[1];


        //}
        //else if (data[0] > 0 && data[1] > 0)
        //{
        //    working_arr[0] = data[0];
        //    working_arr[1] = data[1];
        //    working_arr[2] = active_data[0];
        //    working_arr[3] = active_data[1];
        //}
        //else
        //{
        //    printf("edge case?\n");
        //    working_arr[0] = active_data[0];
        //    working_arr[1] = active_data[1];
        //}
        //   

        /* reset active data */
        active_data = empty_data;
        
        printf("arr = ");

        for (int i = 0; i < get_size(working_arr); i++)
        {
            printf("%d\t", working_arr[i]);
        }
        printf("\n");

        /*
            separate array into lower end and higher end
            if smaller than pivot, 
        */
        lower_count = 0;
        upper_count = 0;

        for (int l = 0; l < size*2; l++)
        {
            lower[l] = -1;
            upper[l] = -1;
        }

        for (int i = 0; i < size*2; i++)
        {
            if (d == 1)
            {
                printf("\t\t\t%d ? %d\n", working_arr[i], pivot);
            }

            if (working_arr[i] < pivot)
            {

                lower[lower_count] = working_arr[i];
                lower_count++;
            }
            else if (working_arr[i] >= pivot)
            {

                upper[upper_count] = working_arr[i];
                upper_count++;

            }
        }
           
 

        /* keep lower if child_id == 0*/

        if (child_id == 0)
        {
            active_data = lower;
        }
        else
        {
            active_data = upper;
        }
        

        

        printf("[%d] active arr:\t{", d);

        for (int i = 0; i < get_size(active_data); i++)
        {
            printf("%d, \t", active_data[i]);
        }
        printf("}\n\n\n");

        


    }

    MPI_Finalize();
    return 0;
}
