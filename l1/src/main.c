#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
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

#define HORSES 5
#define TRACK 50

int track[HORSES] = {0};
int finish_order[HORSES];
int finish_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* horse_run(void* arg) {
  int id = *((int*)arg);
  free(arg);

  while (1) {
    int step = rand() % 4;  // 0 to 3
    usleep(10000);          // simulate gallop

    pthread_mutex_lock(&mutex);
    if (track[id] < TRACK) {
      track[id] += step;
      if (track[id] > TRACK) track[id] = TRACK;
      printf("Horse %d moves to %d\n", id, track[id]);

      if (track[id] >= TRACK) {
        finish_order[finish_count++] = id;
        printf("Horse %d finished the race!\n", id);
        pthread_mutex_unlock(&mutex);
        break;
      }
    }
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

int main() {
  srand(time(NULL));
  pthread_t horses[HORSES];

  for (int i = 0; i < HORSES; i++) {
    int* id = malloc(sizeof(int));
    *id = i;
    pthread_create(&horses[i], NULL, horse_run, id);
  }

  for (int i = 0; i < HORSES; i++) {
    pthread_join(horses[i], NULL);
  }

  printf("Final results\n");
  for (int i = 0; i < HORSES; i++) {
    printf("Place %d: Horse %d\n", i + 1, finish_order[i]);
  }

  return 0;
}