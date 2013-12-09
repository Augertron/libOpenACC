
#ifndef OPENACC_UTILS_TIMER
#define OPENACC_UTILS_TIMER

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct acc_timer_t_ {
  struct timespec start;
  struct timespec stop;
  long delta;
} * acc_timer_t;

acc_timer_t acc_timer_build();

void acc_timer_start(acc_timer_t timer);
void acc_timer_stop (acc_timer_t timer);
void acc_timer_delta(acc_timer_t timer);

#ifdef __cplusplus
}
#endif

#endif

