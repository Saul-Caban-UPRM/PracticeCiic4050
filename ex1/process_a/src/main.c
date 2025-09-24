#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "functions.h"
mqd_t queue;
pid_t child_B;
pid_t child_C;
float *arr = NULL;

void handle_sigint(int sig) {
  mq_close(queue);
  munmap(arr, 25 * sizeof(float));
  kill(child_B, SIGINT);
  kill(child_C, SIGINT);
  kill(getpid(), SIGTERM);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <queue_name> <shm_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  struct sigaction sa;

  sa.sa_handler = handle_sigint;
  sa.sa_flags = 0;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction failed\n");
    exit(1);
  }

  // Messagequeue [1]  shm[2]
  int fd = shm_open(argv[2], O_CREAT | O_RDWR, 0666);
  ftruncate(fd, 25 * sizeof(float));
  arr =
      mmap(NULL, 25 * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (arr == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
  }
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 5;
  attr.mq_msgsize = 30;

  queue = mq_open(argv[1], O_CREAT | O_RDWR, 0666, &attr);
  if (queue == (mqd_t)-1) {
    perror("mq_open failed");
    exit(EXIT_FAILURE);
  }

  child_B = fork();

  if (child_B == 0) {
    execlp("./process_b", "process_b", argv[1], argv[2], NULL);
    perror("execlp process_b failed");
    exit(1);
  }

  child_C = fork();

  if (child_C == 0) {
    execlp("./process_c", "process_c", argv[2], NULL);
    perror("execlp process_c failed");
    exit(1);
  }
  while (1) {
    printf("1. Square numbers\n2. Divide numbers by two\n3. Plot numbers\n");
    int num;
    if (scanf("%d", &num) != 1) {
      printf("Invalid input!\n");
      while (getchar() != '\n');  // clear buffer
      continue;
    }  // continue to read users input untill it gets it right xD
    if (num == 1) {
      mq_send(queue, "Square", sizeof("Square"), 0);
    }
    if (num == 2) {
      mq_send(queue, "half", sizeof("half"), 0);
    }
    if (num == 3) {
      kill(child_C, SIGUSR1);
    }
  }
  return 0;
}