
#include "OpenACC/internal/allocator.h"

#include <assert.h>

struct acc_data_allocations_t_ data_allocations; /// \todo static init

d_void * acc_get_deviceptr(unsigned device_idx, h_void * host_ptr) {
  assert(0);
}

h_void * acc_get_hostptr(unsigned device_idx, d_void * device_ptr) {
  assert(0);
}

int acc_check_present(unsigned device_idx, h_void * host_ptr, size_t n) {
  assert(0);
}

