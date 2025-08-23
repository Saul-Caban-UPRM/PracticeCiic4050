#include <stdio.h>
#include <stdlib.h>

#include "../include/functions.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return 1;
  }

  int test_case = atoi(argv[1]);

  if (test_case >= 1 && test_case <= 5) {
    TellJoke(test_case);
  } else if (test_case >= 6 && test_case <= 10) {
    GivePunchline(test_case - 5);
  } else {
    GivePunchline(0);  // Test invalid input.
  }

  return 0;
}