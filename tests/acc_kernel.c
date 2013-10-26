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

// When loading libraries coded with OpenACC, we want the kernel ID to be defined dynamically (to avoid collision) ("0x00000000" is a pointer fro compilation used to create a unique name)
extern size_t region_0x00000000_id;
extern size_t kernel_0x00000000_id;

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

  acc_init_(acc_device_i7_3610QM, 0);

  unsigned long region_0_num_gang = 16;     // clause num_gang(16)
  unsigned long region_0_num_worker = 64;   // clause num_worker(64)
  unsigned long region_0_vector_length = 1; // default

  // construct parallel region descriptor
  acc_region_t region_0 = acc_build_region(region_0x00000000_id, 1, &region_0_num_gang, &region_0_num_worker, region_0_vector_length);

  acc_region_start(region_0);

    // Create a kernel descriptor
    acc_kernel_t kernel_0 = acc_build_kernel(kernel_0x00000000_id);

    // Enqueue the kernel for the current region
    acc_enqueue_kernel(region_0, kernel_0);

  acc_region_stop(region_0);

  return 0;
}

/*! @} */

