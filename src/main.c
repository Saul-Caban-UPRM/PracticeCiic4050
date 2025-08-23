#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main() {
  int user_number;
  for (int i = 0; i < 3; i++) {
    int random_number =
        ((rand() % 5) +
         1);  // gives a number from 0 to 4. Adding 1 makes it 1 to 5
    TellJoke(random_number);
    printf("Enter a number (1-5):\n");
    scanf("%d", &user_number);
    GivePunchline(user_number);
  }

  return 0;
}