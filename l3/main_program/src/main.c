#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid1 = fork();
  if (pid1 == 0) {
    execl("../../worker/build/worker", "worker", "/prime_queue", NULL);
  }
  pid_t pid2 = fork();
  if (pid2 == 0) {
    execl("../../worker/build/worker", "worker", "/prime_queue", NULL);
  }
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 5;
  attr.mq_msgsize = 40;
  mqd_t queue;

  queue = mq_open("/prime_queue", O_CREAT | O_RDWR, 0666, &attr);

  FILE *file = fopen(
      "/home/fslabs/os_070_2/GuthubLabs/PracticeCiic4050/test/numbers.txt",
      "r");

  char buffer[41];

  while (f) fclose(file);
  sleep(1);

  mq_send(queue, "-1", 3, 0);
  mq_send(queue, "-1", 3, 0);

  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  mq_close(queue);
  mq_unlink("/prime_queue");
  return 0;
}