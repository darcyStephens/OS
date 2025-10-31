
/**
 * This file implements parallel mergesort.
 */

#include "mergesort.h"

#include <stdio.h>
#include <stdlib.h> /* for malloc */
#include <string.h> /* for memcpy */

#define MAX_LEVEL 12

// print array for debugging purposes
void print_array_(int left, int right, int* array) {
  while (left <= right) {
    printf("%d, ", array[left]);
    left++;
  }
  printf("\n");
}

/* this function will be called by mergesort() and also by parallel_mergesort().
 */
void merge(int leftstart, int leftend, int rightstart, int rightend) {
  // copy the contents of A into B for inplace & stable sortnig
  memcpy(&B[leftstart], &A[leftstart],
         (rightend - leftstart + 1) *
             sizeof(int));  // ensure theres enough space to fit the whole array

  int l = leftstart;
  int r = rightstart;
  int k = leftstart;  // target index to copy into, refers to A

  // iterate through B, compare and place smaller elements back into A
  while (l <= leftend && r <= rightend) {
    if (B[l] <= B[r]) {  // if left elem is smaller than right elem, copy left
                         // elem into A
      A[k] = B[l];
      l++;
    } else {  // otherwise, copy right elem into A
      A[k] = B[r];
      r++;
    }
    // move to next target index in A
    k++;
  }

  // if there are remaining elements in the left/right subarrays, copy them back
  // into A
  while (l <= leftend) {
    A[k] = B[l];
    l++;
    k++;
  }

  while (r <= rightend) {
    A[k] = B[r];
    r++;
    k++;
  }
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right) {
  if (left >= right) return;  // base case: array of size 1/0 is sorted already

  int mid = (left + right) / 2;

  // recursively sort left and right sub arrays
  my_mergesort(left, mid);
  my_mergesort(mid + 1, right);

  // merge the sorted sub arrays
  //sorting is happening through merge as you go back up the recursive stack
  merge(left, mid, mid + 1, right);
}

// this function implements parallel mergesort using pthreads, expects a struct
// arg
void* parallel_mergesort(void* arg) {
  struct argument* args =
      (struct argument*)arg;  // cast void pointer to argument to read vals

  // arg vals copied to local variables for readability
  int left = args->left;
  int right = args->right;
  int level = args->level;

  if (left >= right)
    return NULL;  // array of size 1/0 is sorted, return to conserve resources

  /* condition: switch from parallel to sequential mergesort if cutoff/
  max_level is met to avoid threads overloading the system  since 2^12=4096
  threads are created which is near the maximum for a single process  */
  if (level >= cutoff || level >= MAX_LEVEL) {
    my_mergesort(left, right);
    return NULL;
  }

  int mid = (left + right) / 2;

  // build struct arguments for left and right sub arrays, pass in incremented
  // level to reflect depth
  struct argument* leftArg = buildArgs(left, mid, level + 1);
  struct argument* rightArg = buildArgs(mid + 1, right, level + 1);

  // intialise threads for left and right
  pthread_t leftThread, rightThread;

  // create threads for left and right sub arrays
  // thread, attributes (deafult= NULL), function to execute, function arguments
  // threads operate on their function, locks aren't needed since each thread is
  // operating on a different side of the array not accessing same values
  pthread_create(&leftThread, NULL, parallel_mergesort, (void*)leftArg);
  pthread_create(&rightThread, NULL, parallel_mergesort, (void*)rightArg);

  // wait for both threads to finish
  pthread_join(leftThread, NULL);
  pthread_join(rightThread, NULL);

  // merge the sorted sub arrays
  merge(left, mid, mid + 1, right);

  return NULL;  // no need to return anything since sorting is done in place
                // (and is required to match pthreads API & function signature)
}

/* we build the argument for the parallel_mergesort function. */
struct argument* buildArgs(int left, int right, int level) {
  // intialise memory for argument struct
  // cast allocate memory and cast to struct agument pointer
  struct argument* arg = (struct argument*)malloc(sizeof(struct argument));

  // catch malloc failure and return error message + exit process
  if (arg == NULL) {
    fprintf(stderr, "Memory allocation failed for argument struct\n");
    exit(1);
  }
  // assign values of the struct arg.
  //  -> because its a pointer
  arg->left = left;
  arg->right = right;
  arg->level = level;

  return arg;
}
