#include "aco.h"

static unsigned long long time_start[num_timers] = {0};
static unsigned long long time_elapsed[num_timers] = {0};

void timer_start(timer timer) {
  time_start[timer] = rdtsc();
}

void timer_stop(timer timer) {
  time_elapsed[timer] += rdtsc() - time_start[timer];
}

double timer_get(timer timer) {
  return time_elapsed[timer] / CLOCK_RATE;
}

