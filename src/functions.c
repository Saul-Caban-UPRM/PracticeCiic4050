#include "functions.h"

#include <stdio.h>

void SortArray(int *arr, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}
double CalculateMedian(int *arr, int size) {
  if (size % 2 == 0) {
    return (arr[(size - 1) / 2] + arr[size / 2]) / 2.0;

  } else {
    return arr[(size / 2)];
  }
  return 0;
}
double CalculateMean(int *arr, int size) {
  double temp_sum = 0;
  for (int i = 0; i < size; i++) {
    temp_sum += arr[i];
  }
  return temp_sum / size;
}