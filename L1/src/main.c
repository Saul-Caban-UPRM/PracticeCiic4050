#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include "../include/functions.h"

// Declare external functions
int SetNiceValue(int nice_val);
int SetCpuAffinity(int cpu_num);
void* HeavyCpuTask(void* arg);

// --- Main Program ---

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <nice_value> <cpu_number>\n", argv[0]);
    fprintf(stderr, "Example: %s 10 1\n", argv[0]);
    return 1;
  }

  int nice_val = atoi(argv[1]);
  int cpu_num = atoi(argv[2]);
  pthread_t thread;

  // Log the process start for the test script
  printf("TEST_START|PID=%d|NICE=%d|CPU=%d\n", getpid(), nice_val, cpu_num);

  // Set nice value and CPU affinity in the main process before creating the
  // thread
  SetNiceValue(nice_val);
  SetCpuAffinity(cpu_num);

  // Brief pause to ensure external test harness can observe the process
  // (the test script sleeps ~0.4s before querying). Sleeping here keeps
  // the process alive and with the correct settings for inspection.
  usleep(500000);  // 500 ms

  // Prepare thread arguments
  ThreadArgs* args = malloc(sizeof(ThreadArgs));
  args->nice_val = nice_val;
  args->cpu_num = cpu_num;

  // Create the thread
  pthread_create(&thread, NULL, HeavyCpuTask, args);
  pthread_join(thread, NULL);

  free(args);

  printf("TEST_END\n");

  return 0;
}
