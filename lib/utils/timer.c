
#include <time.h>

long delta_timer(struct timespec * timer_start, struct timespec * timer_stop) {
  long delta_ns = timer_stop->tv_nsec - timer_start->tv_nsec;
  if (delta_ns >= 0) {
    delta_ns += (timer_stop->tv_sec - timer_start->tv_sec) * 1000000000;
  }
  else {
    delta_ns = (timer_stop->tv_sec - timer_start->tv_sec) * 1000000000 + delta_ns;
  }
  return delta_ns;
}

