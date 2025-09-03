#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

int main(int argc, char* argv[]) {
  double answer;
  if (argc != 3) {
    fprintf(stderr, "Error: Incorrect number of arguments\n");
    return EXIT_FAILURE;
  } else if (!strcmp(argv[1], "circle_area")) {
    answer = CalculateCircleArea(atof(argv[2]));
    printf("%f\n", answer);
  } else if (!strcmp(argv[1], "sphere_volume")) {
    answer = CalculateSphereVolume(atof(argv[2]));
    printf("%f\n", answer);
  } else if (!strcmp(argv[1], "circle_perimeter")) {
    answer = CalculateCirclePerimeter(atof(argv[2]));
    printf("%f\n", answer);
  } else {
    fprintf(stderr, "Error: Invalid command\n");
    return EXIT_FAILURE;
  }

  return 0;
}