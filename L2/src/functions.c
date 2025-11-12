#include "../include/functions.h"
double mean_x;
double mean_y;

int ExtractD(struct input_event* events, int count, int* dx, int* dy) {
  int prev_x = -1;
  int prev_y = -1;
  int current_x = -1;
  int n_deltas = 0;

  for (int i = 0; i < count; i++) {
    if (events[i].type == EV_ABS) {
      if (events[i].code == ABS_X) {
        current_x = events[i].value;
      } else if (events[i].code == ABS_Y) {
        if (current_x >= 0 && prev_x >= 0 && prev_y >= 0) {
          dx[n_deltas] = current_x - prev_x;
          dy[n_deltas] = events[i].value - prev_y;
          n_deltas++;
        }
        prev_x = current_x;
        prev_y = events[i].value;
      }
    }
  }
  /* Calcular media de los deltas */
  double sum_x = 0.0;
  double sum_y = 0.0;

  for (int i = 0; i < n_deltas; i++) {
    sum_x += dx[i];
    sum_y += dy[i];
  }

  mean_x = sum_x / n_deltas;
  mean_y = sum_y / n_deltas;
  return n_deltas;
}

double CalculateVariance(struct input_event* events, int count) {
  int dx[BUFFER_SIZE];
  int dy[BUFFER_SIZE];
  // If there are too few events, variance cannot be computed
  if (count < 4) {
    return 0.0;
  }

  int n = ExtractD(events, count, dx, dy);
  if (n <= 0) {
    return 0.0;
  }

  double sum = 0.0;
  for (int i = 0; i < n; i++) {
    double dx_diff = dx[i] - mean_x;
    double dy_diff = dy[i] - mean_y;
    sum += dx_diff * dx_diff + dy_diff * dy_diff;
  }

  return sum / (n * 2.0);
}

int BotBehavior(struct input_event* events, int count, double* variance) {
  if (count < 4) {
    if (variance) *variance = 0.0;
    return 0; /* Not enough data, assume human (no alert) */
  }

  double v = CalculateVariance(events, count);
  if (variance) *variance = v;

  if (v < VARIANCE_THRESHOLD) {
    return 1; /* BOT */
  }
  return 0; /* HUMAN */
}
