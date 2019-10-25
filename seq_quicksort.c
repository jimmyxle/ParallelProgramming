/*
    quicksort
        Code written from looking at pseudocode from geeksforgeeks.com
 */
 #include <stdio.h>
 #include <time.h>

 void swap_num1(int* low, int* high)
 {
     int temp = *low;
     *low = *high;
     *high = temp;
 }

void qsort(int arr[], int q, int r)
{
    int x, s;
    if (q < r)
    {
        x = arr[q];
        s = q;
        for (int i = q+1; i <= r; i++)
        {
            if(arr[i]<=x)
            {
                s = s + 1;
                swap_num1(&arr[s], &arr[i]);
            }
        }
        swap_num1(&arr[q], &arr[s]);
        qsort(arr, q, s);
        qsort(arr, s+1, r);
    }
}



int partition(int array[], int low, int high)
{
    int pivot = array[high];
    int lower = low -1;
    for (int j = low; j<= high-1; j++)
    {
        if (array[j]<pivot)
        {
            lower++;
            swap_num1(&array[lower], &array[j]);
        }
    }
    swap_num1(&array[lower+1], &array[high]);
    return lower + 1;
}

void quicksort(int array[], int low, int high)
{
    /*get the partition index, two partitions, call quicksort on both partitions */
    int partition_index;
    if (low < high)
    {
        partition_index = partition(array, low, high);
        quicksort(array, low, partition_index-1);
        quicksort(array, partition_index+1, high);
    }
}
void print_array(int arr[], int size)
{
    for (int i = 0; i < size; i++)
        printf("%d ",arr[i]);
    printf("\n");
}
/*
int main(void)
{
    clock_t start, end;
    int arr[] = {9,25,16,1,4,100,64,49,36,81};
    int arr2[] = {9,25,16,1,4,100,64,49,36,81};

    int n = sizeof(arr) / sizeof(arr[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);

  
    printf("quicksort\n");
    printf("Array before sorting: ");
    print_array(arr, n);
    start = clock();
    quicksort(arr, 0, n-1);
    end = clock();
    printf("Array after sorting:  ");
    print_array(arr, n);

    printf("Duration: %f s\n", ((double)(end - start) / CLOCKS_PER_SEC));

    printf("qsort\n");
    start = clock();
    qsort(arr2, 0, n2-1);
    end = clock();
    printf("Array after sorting:  ");
    print_array(arr2, n2);

    printf("Duration: %f s\n", ((double)(end - start) / CLOCKS_PER_SEC));

    return 0;
}
*/