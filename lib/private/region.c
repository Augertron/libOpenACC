
#include "OpenACC/openacc.h"
#include "OpenACC/private/region.h"
#include "OpenACC/internal/region.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <stdlib.h>

#include <assert.h>

typedef struct acc_region_desc_t_ * acc_region_desc_t;
typedef struct acc_region_t_ * acc_region_t;

struct acc_region_t_ * acc_build_region(acc_region_desc_t region) {
  acc_init_once();

  unsigned num_devices = region->num_devices;

  assert(num_devices > 0); /// \todo case when all present devices are used.

  struct acc_region_t_ * result = (struct acc_region_t_ *)malloc(sizeof(struct acc_region_t_) + num_devices * sizeof(struct acc_region_per_device_t_));

  result->desc = region;
  result->num_devices = num_devices;
  result->distributed_data = (h_void **)malloc(region->num_distributed_data * (sizeof(h_void *) + sizeof(size_t)));

  if (region->devices == NULL) {
    assert(num_devices == 1);

    acc_device_t dev_type = acc_get_device_type();
    int dev_num = acc_get_device_num(dev_type);

    result->devices[0].device_idx = acc_get_device_idx(dev_type, dev_num);
    result->devices[0].num_gang = 0;
    result->devices[0].num_worker = 0;
    result->devices[0].vector_length = 0;
  }
  else {
    unsigned i;
    for (i = 0; i < num_devices; i++) {
      result->devices[i].device_idx = acc_get_device_idx(region->devices[i].kind, region->devices[i].num);
      result->devices[i].num_gang = 0;
      result->devices[i].num_worker = 0;
      result->devices[i].vector_length = 0;
    }
  }

  return result;
}

void acc_region_start(acc_region_t region) {
  acc_region_init(region);

  acc_get_region_defaults(region);

  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue);
  }
}

void acc_region_stop(acc_region_t region) {
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue);
  }
}

