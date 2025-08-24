#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main() {
  Hurricane hurricanes;
  double *user_hurricane;  // array of 3 elements
  printf("Enter a hurricane category (1-5), latitude, and longitude\n");
  user_hurricane = malloc(3 * sizeof(int));
  for (int i = 0; i < 3; i++) {
    scanf("%lf", &user_hurricane[i]);
  }
  if (user_hurricane[0] < 1 || user_hurricane[0] > 5) {
    return EXIT_FAILURE;
  }
  hurricanes =
      CreateHurricane(user_hurricane[0], user_hurricane[1], user_hurricane[2]);
  FillWindSpeeds(&hurricanes);

  printf("Category: %d\n", hurricanes.category);
  printf("Coordinates: %.1f, %.1f\n", hurricanes.latitude,
         hurricanes.longitude);
  // for (int i = 0; i < 100; i++) {
  //   printf("This is the wind speeds %d\n", hurricanes.wind_speeds[i]);
  // }

  return 0;
}