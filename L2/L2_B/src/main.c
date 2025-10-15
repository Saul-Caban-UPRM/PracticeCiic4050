#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <math.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int Is_Prime(int n) {
  if (n < 2) return 0;
  for (int i = 2; i <= sqrt(n); i++) {
    if (n % i == 0) return 0;
  }
  return 1;
}

void* sum_primes(void* arg) {
  int thread_id = *(int*)arg;
  free(arg);  // Free the allocated memory
  int start;
  if (thread_id == 0) {
    start = 2;
  } else {
    start = 500001;
  }
  int end;
  if (thread_id == 0) {
    end = 500000;
  } else {
    end = 1000000;
  }
  int64_t* result = malloc(2 * sizeof(int64_t));
  result[0] = 0;
  result[1] = 0;

  for (int i = start; i <= end; i++) {
    if (Is_Prime(i)) {
      result[0] += i;
      result[1]++;
    }
  }

  return result;
}

int main() {
  pthread_t Threads[2];
  int64_t total_sum = 0;
  int total_count = 0;

  for (int i = 0; i < 2; i++) {
    int* id = malloc(sizeof(int));
    *id = i;
    pthread_create(&Threads[i], NULL, sum_primes, id);
  }

  for (int i = 0; i < 2; i++) {
    void* ret_val;
    pthread_join(Threads[i], &ret_val);
    int64_t* res = (int64_t*)ret_val;
    total_sum += res[0];
    total_count += res[1];
    free(res);
  }

  printf("Total sum of primes between 1 and 1,000,000: %ld\n", total_sum);
  printf("Total number of primes between 1 and 1,000,000: %d\n", total_count);
  return 0;
}
