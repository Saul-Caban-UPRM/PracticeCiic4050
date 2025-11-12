#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

int N;
int P;
int* first_block;
pthread_mutex_t lock;
bool is_five = false;

void Set_cpu_affinity(pthread_t thread, int cpu_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_id, &cpuset);  // CPU 1 in your case

  if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
    perror("pthread_setaffinity_np failed");
  }
}

void Set_nice(int nice_val) {
  if (setpriority(PRIO_PROCESS, 0, nice_val) != 0) {
    perror("setpriority failed");
  }
}

void* SearchQuest(void* arg) {
  int id = *(int*)arg;
  Set_nice(id);

  usleep(200000);

  while (!is_five) {
    int random_number = random() % N;  // Random number between 0 and N-1
    if (first_block[random_number] == 5) {
      pthread_mutex_lock(&lock);
      for (int i = 0; i < P; i++) {
        first_block[i] = -1;
      }
      for (int i = P; i < N; i++) {
        first_block[i] = 1;
      }
      is_five = true;
      pthread_mutex_unlock(&lock);

    } else if (first_block[random_number] == 0) {
      usleep(50000);
    }
  }
  return NULL;
}

// --- Main Program ---
int main(int argc, char* argv[]) {
  srandom(time(NULL));
  pthread_mutex_init(&lock, NULL);  // at the start of main
  N = atoi(argv[1]);
  P = atoi(argv[2]);
  first_block = mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  for (int i = 0; i < N; i++) {
    first_block[i] = 0;  // start at zero
  }
  first_block[P] = 5;

  pthread_t th1, th2;
  int* arg1 = malloc(sizeof(int));
  int* arg2 = malloc(sizeof(int));
  *arg1 = 3;
  *arg2 = 19;  // EXPECTED TO BE A VOID*
  pthread_create(&th1, NULL, SearchQuest, (void*)(arg1));
  pthread_create(&th2, NULL, SearchQuest, (void*)(arg2));

  Set_cpu_affinity(th1, 2);  // CPU 3
  Set_cpu_affinity(th2, 2);  // CPU 19

  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  printf("FINAL ARRAY: ");
  for (int i = 0; i < N; i++) {
    printf("%d ", first_block[i]);
  }

  int count_negative_one = 0;
  int count_one = 0;
  for (int i = 0; i < N; i++) {
    if (first_block[i] == -1) {
      count_negative_one++;
    } else if (first_block[i] == 1) {
      count_one++;
    }
  }
  // printf("negative Ones %d\n", count_negative_one);
  // printf("Ones %d\n", count_one);

  pthread_mutex_destroy(&lock);  // At the end of main
}
