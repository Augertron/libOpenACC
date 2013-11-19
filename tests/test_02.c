/*!
 * \addtogroup grp_libopenacc_tests
 * @{
 *
 * \file tests/acc_kernel.c
 * 
 * In this file, we tests:
 *
 */

#include <math.h>

#include <stdio.h>

#include <assert.h>

#include "OpenACC/openacc.h"
#include "OpenACC/private/debug.h"
#include "OpenACC/private/region.h"
#include "OpenACC/private/kernel.h"
#include "OpenACC/private/loop.h"
#include "OpenACC/private/data-env.h"

#include <time.h>

typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_region_t_ * acc_region_t;

extern struct acc_kernel_desc_t_ kernel_0x00_desc;
extern struct acc_region_desc_t_ region_0x00_desc;

const unsigned long n = 8*64*1*96*1024;

float a[n];
float b[n];
float c[n];

void print_timer(struct timespec * timer_start, struct timespec * timer_stop) {
  long delta_ns = timer_stop->tv_nsec - timer_start->tv_nsec;
  if (delta_ns >= 0) {
    delta_ns += (timer_stop->tv_sec - timer_start->tv_sec) * 1000000000;
  }
  else {
    delta_ns = (timer_stop->tv_sec - timer_start->tv_sec - 1) * 1000000000 + delta_ns;
  }
  printf("timer = %d\n", delta_ns);
}

int main() {
  unsigned i;

  for (i = 0; i < n; i++) {
    a[i] = rand();
    b[i] = rand();
    c[i] = 0.;
  }

  acc_set_device_type(acc_device_any);
  acc_set_device_num(0, acc_device_any);

  // Push a new data environment for region_0
  acc_push_data_environment();

  acc_copyin(a, n * sizeof(float)); // clause copyin(a[0:n])
  acc_copyin(b, n * sizeof(float)); // clause copyin(b[n])
  acc_present_or_create(c, n * sizeof(float)); // allocation for : clause copyout(c)

  struct timespec timer_start;
  clock_gettime(CLOCK_REALTIME, &timer_start);

  unsigned long region_0_num_gang = 8;     // clause num_gang(16)
  unsigned long region_0_num_worker = 64;   // clause num_worker(64)
  unsigned long region_0_vector_length = 1; // clause vector_length(1)

  // construct and start parallel region descriptor
  acc_region_t region_0 = acc_build_region(&region_0x00_desc, 1, &region_0_num_gang, &region_0_num_worker, region_0_vector_length);
  acc_region_start(region_0);

  // Create a kernel descriptor
  acc_kernel_t kernel_0 = acc_build_kernel(&kernel_0x00_desc);

  // Set data arguments
  kernel_0->data_ptrs[0] = acc_deviceptr(a);
  kernel_0->data_ptrs[1] = acc_deviceptr(b);
  kernel_0->data_ptrs[2] = acc_deviceptr(c);

  // Configure the loop
  kernel_0->loops[0]->lower = 0;
  kernel_0->loops[0]->upper = n;
  kernel_0->loops[0]->stride = 1;

  // Enqueue the kernel for the current region
  acc_enqueue_kernel(region_0, kernel_0);

  acc_region_stop(region_0);

  struct timespec timer_stop;
  clock_gettime(CLOCK_REALTIME, &timer_stop);

  print_timer(&timer_start, &timer_stop);

  acc_copyout(c, n * sizeof(float)); // clause copyout(c)

  acc_pop_data_environment();

  for (i = 0; i < 10; i++) {
    printf("%e + %e = %e\n", a[i], b[i], c[i]);
  }

  return 0;
}

/*! @} */

