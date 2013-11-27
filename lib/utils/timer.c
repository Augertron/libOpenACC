
#include <time.h>

long delta_timer(struct timespec * timer_start, struct timespec * timer_stop) {
  long delta_ms = (timer_stop->tv_nsec - timer_start->tv_nsec) / 1000000;
  if (delta_ms >= 0) {
    delta_ms += (timer_stop->tv_sec - timer_start->tv_sec) * 1000;
  }
  else {
    delta_ms = (timer_stop->tv_sec - timer_start->tv_sec) * 1000 + delta_ms;
  }
  return delta_ms;
}

