/*!
 * \addtogroup grp_libopenacc_tests
 * @{
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

#include "OpenACC/utils/timer.h"

typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_region_t_ * acc_region_t;

extern struct acc_kernel_desc_t_ kernel_0x00_desc;
extern struct acc_region_desc_t_ region_0x00_desc;

int main(int argc, char ** argv) {
  assert(argc == 4);

  unsigned long n = atoi(argv[1]);

  struct timespec data_start;
  struct timespec data_stop;
  struct timespec computation_start;
  struct timespec computation_stop;

  unsigned i;

  float * a = (float*)malloc(n * sizeof(float));
  float * b = (float*)malloc(n * sizeof(float));
  float * c = (float*)malloc(n * sizeof(float));

  for (i = 0; i < n; i++) {
    a[i] = rand();
    b[i] = rand();
    c[i] = 0.;
  }

  acc_set_device_type(acc_device_any);
  acc_set_device_num(0, acc_device_any);

  clock_gettime(CLOCK_REALTIME, &data_start);

  // Push a new data environment for region_0
  acc_push_data_environment();

  acc_copyin(a, n * sizeof(float)); // clause copyin(a[0:n])
  acc_copyin(b, n * sizeof(float)); // clause copyin(b[n])
  acc_present_or_create(c, n * sizeof(float)); // allocation for : clause copyout(c)

  clock_gettime(CLOCK_REALTIME, &computation_start);

  unsigned long region_0_num_gang = atoi(argv[2]);
  unsigned long region_0_num_worker = atoi(argv[3]);
  unsigned long region_0_vector_length = 1;

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

  clock_gettime(CLOCK_REALTIME, &computation_stop);

  acc_copyout(c, n * sizeof(float));

  acc_pop_data_environment();

  clock_gettime(CLOCK_REALTIME, &data_stop);

  long delta_computation = delta_timer(&computation_start, &computation_stop);
  long delta_data = delta_timer(&data_start, &data_stop);

  printf("%d %d %d %d %d\n", n, region_0_num_gang, region_0_num_worker, delta_computation, delta_data);

  free(a);
  free(b);
  free(c);

  return 0;
}

/*! @} */

