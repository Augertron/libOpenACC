
#ifndef OPENACC_UTILS_TIMER
#define OPENACC_UTILS_TIMER

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

long delta_timer(struct timespec * timer_start, struct timespec * timer_stop);

#ifdef __cplusplus
}
#endif

#endif

