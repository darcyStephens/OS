
/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
    int left = leftend - leftstart + 1;
    int right = rightend - rightstart;

    int k = 0;

    //copy contents into leftside and rightside of B
    for(int i = leftstart; i < leftend; i++)
    {
        B[k] = A[i];
        k++;
    }
    k = rightstart;
    for(int j = rightstart; j < rightend; j++)
    {
        B[k] = A[j];
        k++;
    }

    //copy contents back into A sorted
    int i = leftstart;
    int j = rightstart;
    k = leftstart;
    while(i < leftend && j < rightend)
    {
        if(B[i] <= B[j])
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

    //copy any remaining contents into A
    while(i < leftend)
    {
        A[k] = B[i];
        i++;
        k++;
    }

     while(j < rightend)
    {
        A[k] = B[j];
        j++;
        k++;
    }
}
/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
    if(left < right)
    {
        int leftstart = 0;
        int leftend = right - left;
        int rightstart = leftend+1;
        int rightend = left + (right-left);

        my_mergesort(leftstart, leftend);
        my_mergesort(rightstart, rightend);
        merge(leftstart, leftend, rightstart, rightend);
    }
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
		return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
		return NULL;
}
