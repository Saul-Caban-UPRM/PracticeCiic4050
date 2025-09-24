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
float *arr = NULL;
int main(int argc, char *argv[]) {
  queue = mq_open(argv[1], O_RDWR, 0666, NULL);  //

  int fd = shm_open(argv[2], O_RDWR, 0666);
  arr =
      mmap(NULL, 25 * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  for (int i = 0; i < 25; i++) {
    arr[i] = 2.0;
  }

  while (1) {
    char buff[30];
    mq_receive(queue, buff, sizeof(buff), NULL);
    if (strcmp(buff, "Square") == 0) {
      for (int i = 0; i < 25; i++) {
        arr[i] *= arr[i];
      }
    }
    if (strcmp(buff, "half") == 0) {
      for (int i = 0; i < 25; i++) {
        arr[i] /= 2;
      }
    }
  }
  return 0;
}