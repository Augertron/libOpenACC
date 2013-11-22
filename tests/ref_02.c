

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <time.h>

#include <assert.h>

#include "OpenACC/utils/timer.h"

int main(int argc, char ** argv) {
  assert(argc == 2);

  unsigned long n = atoi(argv[1]);

  struct timespec start;
  struct timespec stop;

  unsigned i;

  float * a = (float*)malloc(n * sizeof(float));
  float * b = (float*)malloc(n * sizeof(float));
  float * c = (float*)malloc(n * sizeof(float));

  for (i = 0; i < n; i++) {
    a[i] = rand();
    b[i] = rand();
    c[i] = 0.;
  }

  clock_gettime(CLOCK_REALTIME, &start);

  for (i = 0; i < n; i++)
    c[i] = a[i] + b[i];

  struct timespec timer_stop;
  clock_gettime(CLOCK_REALTIME, &stop);

  printf("%d\n", delta_timer(&start, &stop));
/*
  for (i = 0; i < 10; i++) {
    printf("%e + %e = %e\n", a[i], b[i], c[i]);
  }
*/
  free(a);
  free(b);
  free(c);

  return 0;
}

/*! @} */

