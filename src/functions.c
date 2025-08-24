#include "functions.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Hurricane CreateHurricane(int category, double latitude, double longitude) {
  Hurricane hurricane;
  hurricane.category = category;
  hurricane.latitude = latitude;
  hurricane.longitude = longitude;
  return hurricane;
}
void FillWindSpeeds(Hurricane* hurricane) {
  srand(MIN_WIND_SPEED);
  int size = 100;
  for (int i = 0; i < size; i++) {
    if (hurricane->category == 1) {
      int random =
          (rand() % (MAX_SPEED_CAT_1 - MIN_WIND_SPEED + 1)) + MIN_WIND_SPEED;
      hurricane->wind_speeds[i] = random;
    } else if (hurricane->category == 2) {
      int random =
          (rand() % (MAX_SPEED_CAT_2 - MIN_WIND_SPEED + 1)) + MIN_WIND_SPEED;
      hurricane->wind_speeds[i] = random;
    } else if (hurricane->category == 3) {
      int random =
          (rand() % (MAX_SPEED_CAT_3 - MIN_WIND_SPEED + 1)) + MIN_WIND_SPEED;
      hurricane->wind_speeds[i] = random;
    } else if (hurricane->category == 4) {
      int random =
          (rand() % (MAX_SPEED_CAT_4 - MIN_WIND_SPEED + 1)) + MIN_WIND_SPEED;
      hurricane->wind_speeds[i] = random;
    } else {
      int random =
          (rand() % (MAX_SPEED_CAT_5 - MIN_WIND_SPEED + 1)) + MIN_WIND_SPEED;
      hurricane->wind_speeds[i] = random;
    }
  }
}
