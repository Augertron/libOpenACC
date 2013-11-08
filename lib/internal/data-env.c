
#include "OpenACC/internal/data-env.h"
#include "OpenACC/internal/allocator.h"
#include "OpenACC/internal/runtime.h"

#include <assert.h>

struct acc_data_environment_t_ data_environment_  = (struct acc_data_environment_t_){NULL, NULL}; /// \todo finish init
struct acc_data_environment_t_ * data_environment = &data_environment_;

struct acc_data_environment_t_ * acc_build_data_environment(struct acc_data_environment_t_ * parent) {
  struct acc_data_environment_t_ * result = (struct acc_data_environment_t_ *)malloc(sizeof(struct acc_data_environment_t_));

  result->parent = parent;
  result->child = NULL;

  cl_uint num_device = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];

  /// \todo init fields

  return result;
}

void acc_map_data_(unsigned device_idx, h_void * host_ptr, d_void * dev_ptr, size_t n) {
  assert(0);
}

void acc_unmap_data_(unsigned device_idx, h_void * host_ptr) {
  assert(0);
}

