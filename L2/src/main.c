#include <stdio.h>

#include "../include/functions.h"

int fd = 0;
struct input_event buffer[BUFFER_SIZE];
int write_idx = 0;
int count = 0;
int total_events = 0;
int bot_alerts = 0;
pthread_mutex_t mtx;

// Reader thread that captures mouse events
void* Reader(void* arg) {
  while (1) {
    struct input_event ev;
    ssize_t rd = read(fd, &ev, sizeof(struct input_event));
    if (rd != sizeof(struct input_event)) {
      if (rd == -1 && errno == EINTR) continue;
      usleep(1000);
      continue;
    }

    /* Only store absolute X and Y events */
    if (ev.type == EV_ABS && (ev.code == ABS_X || ev.code == ABS_Y)) {
      pthread_mutex_lock(&mtx);
      buffer[write_idx] = ev;
      write_idx = (write_idx + 1) % BUFFER_SIZE;
      if (count < BUFFER_SIZE) count++;
      total_events++;
      pthread_mutex_unlock(&mtx);
    }
  }
  return NULL;
}

// Analyzer thread that periodically checks for patterns
void* Analyzer(void* arg) {
  while (1) {
    sleep(ANALYSIS_INTERVAL);

    /* Copy buffer protected by mutex */
    pthread_mutex_lock(&mtx);
    int current_count = count;
    struct input_event local_buf[BUFFER_SIZE];
    for (int i = 0; i < current_count; i++) {
      int idx = (write_idx - current_count + i);
      while (idx < 0) idx += BUFFER_SIZE;
      idx %= BUFFER_SIZE;
      local_buf[i] = buffer[idx];
    }
    pthread_mutex_unlock(&mtx);

    if (current_count == 0) {
      fprintf(stderr, "\n- No movement detected...\n");
      continue;
    }

    double variance = 0.0;
    int res = BotBehavior(local_buf, current_count, &variance);

    if (res == 1) {
      fprintf(stderr, "\nALERTA BOT DETECTADO\n");
      pthread_mutex_lock(&mtx);
      bot_alerts++;
      int tot = total_events;
      int alerts = bot_alerts;
      pthread_mutex_unlock(&mtx);
      fprintf(stderr, " Varianza: %.2f (umbral: %.2f)\n", variance,
              VARIANCE_THRESHOLD);
      fprintf(stderr, " Events analyzed: %d\n", current_count);
      fprintf(stderr, " Total events: %d | Alertas: %d\n", tot, alerts);
    } else {
      pthread_mutex_lock(&mtx);
      int tot = total_events;
      int alerts = bot_alerts;
      pthread_mutex_unlock(&mtx);
      fprintf(stderr, "\nNormal human behavior\n");
      fprintf(stderr, " Varianza: %.2f (umbral: %.2f)\n", variance,
              VARIANCE_THRESHOLD);
      fprintf(stderr, " Events analyzed: %d\n", current_count);
      fprintf(stderr, " Total events: %d | Alertas: %d\n", tot, alerts);
    }
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <device_or_file>\n", argv[0]);
    return 1;
  }

  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  pthread_mutex_init(&mtx, NULL);

  pthread_t reader_t, analyzer_t;
  pthread_create(&reader_t, NULL, Reader, NULL);
  pthread_create(&analyzer_t, NULL, Analyzer, NULL);

  sleep(ANALYSIS_TIME);

  pthread_cancel(reader_t);
  pthread_cancel(analyzer_t);
  pthread_join(reader_t, NULL);
  pthread_join(analyzer_t, NULL);

  fprintf(stderr, "\n==== FINAL SUMMARY ====\n");
  fprintf(stderr, " Total events captured: %d\n", total_events);
  fprintf(stderr, " Alertas: %d\n", bot_alerts);

  close(fd);
  pthread_mutex_destroy(&mtx);
  return 0;
}
