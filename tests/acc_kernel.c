/*!
 * \addtogroup grp_libopenacc_tests
 * @{
 *
 * \file tests/acc_kernel.c
 * 
 * In this file, we tests:
 *
 */

#include "OpenACC/libopenacc.h"

#include <assert.h>

#ifndef N
# define N 16*64*1*18
#endif

extern struct acc_kernel_desc_t_ kernel_0x00000000_desc;

extern struct acc_region_desc_t_ region_0x00000000_desc;

int main() {
  const unsigned long n = N;

  float a[n];
  float b[n];
  float c[n];

  unsigned i;

  for (i = 0; i < n; i++) {
    a[i] = rand();
    b[i] = rand();
    c[i] = 0.;
  }

  acc_init_(acc_device_any, 0);

  unsigned long region_0_num_gang = 16;     // clause num_gang(16)
  unsigned long region_0_num_worker = 64;   // clause num_worker(64)
  unsigned long region_0_vector_length = 1; // clause vector_length(1)

  // construct parallel region descriptor
  acc_region_t region_0 = acc_build_region(&region_0x00000000_desc, 1, &region_0_num_gang, &region_0_num_worker, region_0_vector_length);

  acc_region_start(region_0);

    // Create a kernel descriptor
    acc_kernel_t kernel_0 = acc_build_kernel(&kernel_0x00000000_desc);

    kernel_0->data_ptrs[0] = NULL;
    kernel_0->data_ptrs[1] = NULL;
    kernel_0->data_ptrs[2] = NULL;

    kernel_0->loops[0]->lower = 0;
    kernel_0->loops[0]->upper = n;
    kernel_0->loops[0]->stride = 1;

    // Enqueue the kernel for the current region
    acc_enqueue_kernel(region_0, kernel_0);

  acc_region_stop(region_0);

  return 0;
}

/*! @} */

