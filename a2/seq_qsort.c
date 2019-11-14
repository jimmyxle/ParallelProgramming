#include <stdio.h>
#include <stdlib.h>


int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}
int main(int argc, char* argv[])
{
	time_t t;

    /* Intializes random number generator */
    srand((unsigned)time(&t));

	if (argc != 2)
			printf("usage: %s <number of workers>\n", argv[0]);
		else
			num_elements = atoi(argv[1]);
		
	array_to_sort = malloc(sizeof(int) * num_elements);
	for (int i = 0; i < num_elements; i++)
		array_to_sort[i] = rand() % 1000;
	
	qsort(recv_n, atoi(argv[1]), sizeof(int), cmpfunc);
	printf("\n sorted \n");
	for (int i = 0; i < atoi(argv[1]); i++)
	{
		int data = recv_n[i];
		printf("%d ", data);
	}
	return 0;

}
