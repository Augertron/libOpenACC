
#include "OpenACC/private/region.h"
#include "OpenACC/internal/region.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <stdlib.h>

#include <assert.h>

typedef struct acc_region_desc_t_ * acc_region_desc_t;
typedef struct acc_region_t_ * acc_region_t;

struct acc_region_t_ * acc_build_region(acc_region_desc_t region, size_t num_dims, size_t * num_gang, size_t * num_worker, size_t vector_length) {
  acc_init_once();

  struct acc_region_t_ * result = (struct acc_region_t_ *)malloc(sizeof(struct acc_region_t_));

  result->num_devices = 0;
  result->device_idx = NULL;

  result->desc          = region;
  result->num_dims      = num_dims;
  result->num_gang      = num_gang;
  result->num_worker    = num_worker;
  result->vector_length = vector_length;

  return result;
}

void acc_region_start(acc_region_t region) {
  region->num_devices = 1;
  region->device_idx = malloc(sizeof(size_t));
  region->device_idx[0] = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_region_init(region);

  acc_get_region_defaults(region);

  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->device_idx[idx]]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->device_idx[idx]]->command_queue);
  }
}

void acc_region_stop(acc_region_t region) {
  assert(region->num_devices != 1 || region->device_idx[0] == acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num));

  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->device_idx[idx]]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->device_idx[idx]]->command_queue);
  }
}

