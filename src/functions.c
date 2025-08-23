#include "functions.h"

#include <stdio.h>

void TellJoke(int number) {
  switch (number) {
    case 1:
      printf("Do you know why the programmer quit his job?\n");
      break;
    case 2:
      printf("Do you know why C is a good language for kids?\n");
      break;
    case 3:
      printf("Do you know why the computer was cold?\n");
      break;

    case 4:
      printf("Do you know why computers are so smart?\n");
      break;

    case 5:
      printf("Do you know why a byte got married?\n");
      break;

    default:
      break;
  }
}

void GivePunchline(int number) {
  // while (number < 1 || number > 5) {
  //   printf("Invalid number. Please enter a number between 1 and 5.");
  //    scanf("%d", number);
  //   }
  if (number < 1 || number > 5) {
    printf("Invalid number. Please enter a number between 1 and 5.");
  }
  switch (number) {
    case 1:
      printf("Because he didn't get arrays.\n");
      break;
    case 2:
      printf("Because it's easy to C!\n");
      break;
    case 3:
      printf("Because it left its Windows open.\n");
      break;
    case 4:
      printf("Because they listen to their motherboards.\n");
      break;
    case 5:
      printf("Because it needed to be converted into a gig-abyte.\n");
      break;
    default:

      break;
  }
}