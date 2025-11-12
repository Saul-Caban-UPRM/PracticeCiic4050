#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define ANALYSIS_INTERVAL 2
#define ANALYSIS_TIME 20
#define VARIANCE_THRESHOLD 50.0

double CalculateVariance(struct input_event* events, int count);
int BotBehavior(struct input_event* events, int count, double* variance);

#endif
