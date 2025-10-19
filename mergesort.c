
/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"


void print_array_(int left, int right, int *array) {
    while (left <= right) {
        printf("%d, ", array[left]);
        left++;
    }
    printf("\n");
}


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend)
{

    //copy the contents of A into B
    int z = leftstart;
    for(z = leftstart; z <= rightend; z++)
    {
        B[z] = A[z];
    }
    int i = leftstart;
    int j = rightstart;
    int k = leftstart;

    // compare and place smaller elements back into A
    //increment the smaller elemen's pointer
    while (i <= leftend && j <= rightend)
    {
        if (B[i] <= B[j])
        {
            A[k] = B[i];
            i++; 
        }
        else
        {
            A[k] = B[j];
            j++;
        }
        k++;
    }

    // clean up remaining elements
    while (i <= leftend)
    {
        A[k] = B[i];
        i++;
        k++;
    }
    while (j <= rightend)
    {
        A[k] = B[j];
        j++;
        k++;
    }
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right)
{
    if (left < right)
    {
        int mid = (left + right) / 2;
        //recursively sort left and right halves
        //recursion magic ngl
        my_mergesort(left, mid);
        my_mergesort(mid + 1, right);
        merge(left, mid, mid + 1, right);
    }
    else
    {
        return;
    }
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg)
{
    struct argument *args = (struct argument *)arg;
    int left = args->left;
    int right = args->right;
    int level = args->level;

    if (level >= cutoff)
    {
        //base case
        //cutoff is where we switch from threads to sequential
    
        my_mergesort(left, right);
        return NULL;
    }
    else
    {
        int mid = (left + right) / 2;

        //build arguments for left and right sub arrays
        struct argument *leftArg = buildArgs(left, mid, level + 1);
        struct argument *rightArg = buildArgs(mid + 1, right, level + 1);

        //intialise our threads for left and right
        pthread_t leftThread, rightThread;

        //create threads for left and right sub arrays
        //thread, attributes (not needed), function to execute, function arguments
        //threads operate on their function, locks aren't needed since each thread is operating on a different side of the array
        //not accessing same values
        pthread_create(&leftThread, NULL, parallel_mergesort, (void *)leftArg);
        pthread_create(&rightThread, NULL, parallel_mergesort, (void *)rightArg);

        

        //wait for both threads to finish
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        //merge the sorted sub arrays
        merge(left, mid, mid + 1, right);

        // //free allocated memory for arguments
        // free(leftArg);
        // free(rightArg);
        return NULL;
    }

    //parellel the sorting of each sub section of the array.
    //number of levels determines number of sections

    return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level)
{
    // intialize memory for argument struct
    //cast allocate memory and cast to struct agument pointer
    struct argument *arg = (struct argument *) malloc(sizeof(struct argument));
    if (arg == NULL)
    {
        fprintf(stderr, "Memory allocation failed for argument struct\n");
        exit(1);
    }
    //-> because its a pointer
    arg->left = left;
    arg->right = right;
    arg->level = level;
    return arg;
}
