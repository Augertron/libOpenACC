
#include "OpenACC/utils/timer.h"

/*
typedef struct acc_timer_t_ {
  struct timespec start;
  struct timespec stop;
  long delta;
} * acc_timer_t;
*/

acc_timer_t acc_timer_build() {
  return malloc(sizeof(struct acc_timer_t_));
}

void acc_timer_start(acc_timer_t timer) {
   if (timer == NULL) return;

  clock_gettime(CLOCK_REALTIME, &(timer->start));
}

void acc_timer_stop (acc_timer_t timer) {
   if (timer == NULL) return;

  clock_gettime(CLOCK_REALTIME, &(timer->stop));
}

void acc_timer_delta(acc_timer_t timer) {
   if (timer == NULL) return;

  timer->delta = (timer->stop.tv_nsec - timer->start.tv_nsec) / 1000000;
  if (timer->delta >= 0)
    timer->delta += (timer->stop.tv_sec - timer->start.tv_sec) * 1000;
  else
    timer->delta = (timer->stop.tv_sec - timer->start.tv_sec) * 1000 + timer->delta;
}

/*
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
*/
