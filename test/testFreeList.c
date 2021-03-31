#include <stdio.h>
#include <time.h>

void initArray(int* arr[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        arr[i] = 0;
    }
}

int valueInArray(int* val, int* arr[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(arr[i] == val)
            return 1;
    }
    return 0;
}

void addToArray(int* val, int* arr[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(arr[i] == 0) {
            arr[i] = val;
            break;
        }  
    }
}

void printArray(int* arr[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        printf("i : %d, val : %p\n", i, arr[i]);
    }
}

int notNullValues(int* arr[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(arr[i] == 0)
            return i;
    }
    return size;
}

int differentArrays(int* arr1[], int* arr2[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(valueInArray(arr1[i], arr2, size) == 0)
            return 1;
    }
    return 0;
}

int differentArraysStrict(int* arr1[], int* arr2[], int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(valueInArray(arr1[i], arr2, size) == 1)
            return 0;
    }
    return 1;
}


int numberValueInCommon(int* arr1[], int* arr2[], int size)
{
    int count = 0;
    int i;
    for(i = 0; i < size; i++)
    {
        if(valueInArray(arr1[i], arr2, size) == 1)
            count++;
    }
    return count;
}

int main(int argc, char *argv[]) {

    int SIZE = 0;

    if( argc == 2 ) {
        printf("The number of pointers tested is %s\n", argv[1]);
        SIZE = atoi(argv[1]);
    }
    else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
        return 1;
    }
    else {
        printf("One argument expected.\n");
        return 1;
    }
    
    int* base = malloc(sizeof(int));
    int* head = malloc(sizeof(int));
    free(head);
    int* first = malloc(sizeof(int));
    free(first);

    int* pointers[SIZE];
    int* seen1[SIZE];
    int* seen2[SIZE];

    initArray(seen1, SIZE);
    initArray(seen2, SIZE);

    
    clock_t difference;
    int msec = 0;
    clock_t before1 = clock();

    printf("Allocating...\n");
    for (int i = 0; i < SIZE; i++)
    {
        pointers[i] = malloc(sizeof(int));
        if (valueInArray(pointers[i], seen1, SIZE) == 0) {
            addToArray(pointers[i], seen1, SIZE);
        }
    }

    difference = clock() - before1;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    printf("Allocation time 1 : %d.%d seconds (%f values per second)\n", msec/1000, msec%1000, (float)SIZE/((float)msec/1000.0f));

    printf("Freeing...\n");
    for (int i = 0; i < SIZE; i++)
    {
        free(pointers[i]);
    }

    difference = clock() - before1;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    printf("Total time 1 : %d.%d seconds (%f values per second)\n", msec/1000, msec%1000, (float)SIZE/((float)msec/1000.0f));


    clock_t before2 = clock();

    printf("Reallocating...\n");
    for (int i = 0; i < SIZE; i++)
    {
        pointers[i] = malloc(sizeof(int));
        if (valueInArray(pointers[i], seen2, SIZE) == 0) {
            addToArray(pointers[i], seen2, SIZE);
        }
    }

    difference = clock() - before2;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    printf("Allocation time 2 : %d.%d seconds (%f values per second)\n", msec/1000, msec%1000, (float)SIZE/((float)msec/1000.0f));


    printf("Freeing...\n");
    for (int i = 0; i < SIZE; i++)
    {
        free(pointers[i]);
    }

    difference = clock() - before2;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    printf("Total time 2 : %d.%d seconds (%f values per second)\n", msec/1000, msec%1000, (float)SIZE/((float)msec/1000.0f));

    difference = clock() - before1;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    printf("\nTotal time of the program : %d.%d seconds\n", msec/1000, msec%1000, (float)SIZE/((float)msec/1000.0f));

    if (differentArrays(seen1, seen2, SIZE) == 0)
        printf("Pointers returned are all the same\n");
    else
        printf("Pointers returned are not all the same\n");

    printf("There are %d pointer(s) in common\n", numberValueInCommon(seen1, seen2, SIZE));

    return 0;
}