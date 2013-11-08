
#include "OpenACC/internal/data-env.h"
#include "OpenACC/internal/allocator.h"
#include "OpenACC/internal/runtime.h"

#include <assert.h>

struct acc_data_environment_t_ data_environment_  = (struct acc_data_environment_t_){NULL, NULL};
struct acc_data_environment_t_ * data_environment = &data_environment_;

struct acc_data_environment_t_ * acc_build_data_environment(struct acc_data_environment_t_ * parent) {
  struct acc_data_environment_t_ * result = (struct acc_data_environment_t_ *)malloc(sizeof(struct acc_data_environment_t_));

  result->parent = parent;
  result->child = NULL;

  cl_uint num_device = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];

  /// \todo init fields

  return result;
}

