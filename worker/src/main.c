#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

bool isPrime(long long n) {
  if (n <= 1) {
    return false;
  }
  for (long long i = 2; i * i <= n; i++) {
    if (n % i == 0) return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <queue_name>\n", argv[0]);
    return 1;
  }

  mqd_t queue = mq_open(argv[1], O_RDWR);
  char buffer[41];
  int priority;

  mq_receive(queue, buffer, sizeof(buffer), &priority);
  printf("buffer: %s\n", buffer);
  // while (1) {
  //   mq_receive(queue, buffer, sizeof(buffer), &priority);
  //   printf("buffer: %s\n", buffer);
  //   num = atoll(buffer);

  //   if (num == -1) break;
  //   if (isPrime(num)) {
  //     fprintf(stderr, "PRIME FOUND: %lld\n", num);
  //   }
  // }
  // mq_close(queue);
  return 0;
}