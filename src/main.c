#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main() {
  int size;
  printf("Enter the size of the array: ");
  scanf("%d", &size);

  int *arr = malloc(size * sizeof(int));

  printf("Enter %d elements:\n", size);
  for (int i = 0; i < size; i++) {
    scanf("%d", &arr[i]);
  }

  SortArray(arr, size);
  printf("Median: %.2f\n", CalculateMedian(arr, size));
  printf("Mean: %.2f\n", CalculateMean(arr, size));
  free(arr);
  return 0;
}