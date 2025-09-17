#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

bool isPrime(long long n) {
  if (n <= 1) return false;
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

  mqd_t queue = mq_open(argv[1], O_RDONLY);
  if (queue == (mqd_t)-1) {
    perror("mq_open");
    return 1;
  }

  char buffer[41];
  while (1) {
    ssize_t bytes = mq_receive(queue, buffer, sizeof(buffer), NULL);
    if (bytes < 0) {
      perror("mq_receive");
      break;
    }
    buffer[bytes] = '\0';
    long long num = atoll(buffer);
    if (num == -1) break;
    if (isPrime(num)) {
      fprintf(stderr, "PRIME FOUND %lld\n", num);
    }
  }

  mq_close(queue);
  return 0;
}