#include "functions.h"

#include <stdint.h>
#include <stdio.h>

void FillArray(int* arr, int size) {
  for (int i = 0; i < size; i++) {
    arr[i] = i + 1;
  }
}

int64_t SumArray(int* arr, int size) {
  int64_t sum = 0;
  for (int item = 0; item < size; item++) {
    sum += arr[item];
  }
  return sum;
}
