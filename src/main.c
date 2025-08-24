#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main() {
  int *arr;
  int user_number;
  int64_t final_sum;
  printf("Enter a number (10-50):\n");
  scanf("%d", &user_number);
  if (user_number < 10 || user_number > 50) {
    return EXIT_FAILURE;
  }
  arr = malloc(user_number * sizeof(int));
  FillArray(arr, user_number);
  final_sum = SumArray(arr, user_number);
  printf("%ld\n", final_sum);
  free(arr);
  return 0;
}