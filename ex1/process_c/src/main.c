#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "functions.h"

float *arr = NULL;
bool print = false;
void handle_signal(int sig) { print = true; }
int main(int argc, char *argv[]) {
  int fd = shm_open(argv[1], O_RDWR, 0666);
  arr =
      mmap(NULL, 25 * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  struct sigaction sa;
  sa.sa_handler = handle_signal;
  sigaction(SIGUSR1, &sa, NULL);
  while (1) {
    pause();
    if (print) {
      for (int i = 0; i < 25; i++) {
        printf("Printing the numbers in the memroy: %f\n", arr[i]);
      }
      print = false;
    }
  }

  return 0;
}