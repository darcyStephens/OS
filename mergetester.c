int B[1000000];  // Temporary array for merging

void merge(int A[], int leftstart, int leftend, int rightstart, int rightend) {
  int left = leftend - leftstart + 1;
  int right = rightend - rightstart + 1;

  // copy contents into leftside and rightside of B
  for (int z = leftstart; z <= rightend; z++) {
    B[z] = A[z];
  }
  int i = leftstart;
  int j = rightstart;
  int k = leftstart;

  // compare and merge smaller elements back into A
  while (i <= leftend && j <= rightend) {
    if (B[i] <= B[j]) {
      A[k] = B[i];
      i++;
    } else {
      A[k] = B[j];
      j++;
    }
    k++;
  }

  // clean up remaining elements
  while (i <= leftend) {
    A[k] = B[i];
    i++;
    k++;
  }
  while (j <= rightend) {
    A[k] = B[j];
    j++;
    k++;
  }
}
#include <stdio.h>
/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int A[], int left, int right) {
  if (left < right) {
    int mid = (left + right) / 2;
    my_mergesort(A, left, mid);
    my_mergesort(A, mid + 1, right);
    merge(A, left, mid, mid + 1, right);
  }
}
int main() {
  int Array[] = {9, 8, 7, 6, 5, 43, 21, 1, 0, -1};
  int size = sizeof(Array) / sizeof(Array[0]);
  my_mergesort(Array, 0, size - 1);
  for (int i = 0; i < size; i++) {
    printf("%d ", Array[i]);
  }
}