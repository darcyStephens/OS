
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
    int right = rightend - rightstart + 1;

    
    //copy contents into leftside and rightside of B
    for(int z = leftstart; z <= rightend; z++)
    {
        B[z] = A[z];
    }
    int i = leftstart;
    int j = rightstart;
    int k = leftstart;
    while(i <= leftend && j <= rightend)
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
    while(i<=leftend)
    {
        A[k] = B[i];
        i++;
        k++;
    }
    while(j<=rightend)
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
      int mid = (left+right)/2;
      my_mergesort(left, mid);
      my_mergesort(mid+1, right); 
      merge(left, mid, mid+1, right);
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
