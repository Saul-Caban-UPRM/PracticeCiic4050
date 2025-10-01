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
double *arr = NULL;
mqd_t queue;
pid_t child1;
pid_t child2;

void Handle_Signal(int signal) {
  printf("Data Processor: new data available\n");
}
int main(int argc, char *argv[]) {
  pid_t father = getpid();
  int fd = shm_open("/mem_block_exam_1", O_CREAT | O_RDWR, 0666);
  ftruncate(fd, 10 * sizeof(double));
  arr = mmap(0, 10 * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (arr == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
  }

  queue = mq_open("/mq_exam_1", O_CREAT | O_RDWR, 0666, NULL);

  if (queue == (mqd_t)-1) {
    perror("mq_open failed");
    exit(EXIT_FAILURE);
  }

  struct sigaction sa;  // Name of variable
  sa.sa_handler = Handle_Signal;
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {  // put the right signal!
    perror("sigaction failed\n");
    exit(1);
  }
  child1 = fork();

  if (child1 == 0) {
    char buff[80];
    snprintf(buff, sizeof(buff), "%d", father);
    execlp("./data_creator", "data_creator", "/mem_block_exam_1", buff, NULL);
  }
  child2 = fork();
  if (child2 == 0) {
    execlp("./data_logger", "data_logger", "/mq_exam_1", NULL);
  }
  int counter = 0;
  while (counter < 5) {
    pause();
    double avr = 0.0;
    for (int i = 0; i < 10; i++) {
      printf("%lf %d\n", arr[i], i);
      avr += arr[i];
    }
    avr = avr / 10;
    char buffer[100];

    snprintf(buffer, sizeof(buffer), "%lf", avr);
    if (mq_send(queue, buffer, strlen(buffer) + 1, 0) == -1) {
      perror("mq_send failed");
    }
    counter++;
  }
  waitpid(child1, NULL, 0);
  waitpid(child2, NULL, 0);

  mq_close(queue);
  mq_unlink("/mq_exam_1");

  munmap(arr, 10);
  shm_unlink("mem_block_exam_1");
}
