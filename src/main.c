#include <stdio.h>
#include <stdlib.h>

#include "array_function.h"

int main() {
  int size;
  printf("Enter the size of the array: ");
  scanf("%d", &size);

  int *arr = malloc(size * sizeof(int));
  if (arr == NULL) {
    printf("Memory allocation failed!\n");
    return 1;
  }

  printf("Enter %d elements:\n", size);
  for (int i = 0; i < size; i++) {
    scanf("%d", &arr[i]);
  }

  SortArray(arr, size);
  printf("Sorted array: ");

  for (int i = 0; i < size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
  printf("Median: %.2f\n", CalculateMedian(arr, size));

  printf("\n");
  free(arr);
  return 0;
}