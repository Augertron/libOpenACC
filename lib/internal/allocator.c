
#include "OpenACC/internal/allocator.h"

#include <assert.h>

struct acc_opencl_allocator_t_ opencl_allocator = (struct acc_opencl_allocator_t_){0, 0, NULL};

struct acc_opencl_data_alloc_t_ * acc_alloc_(unsigned device_idx, h_void * host_ptr, size_t size, acc_opencl_address_spaces_e address_space) {
  assert(0);
}

void acc_free_(struct acc_opencl_data_alloc_t_ * data_alloc) {
  assert(0);
}

d_void * acc_get_deviceptr(unsigned device_idx, h_void * host_ptr) {
  assert(0);
}

h_void * acc_get_hostptr(unsigned device_idx, d_void * device_ptr) {
  assert(0);
}

