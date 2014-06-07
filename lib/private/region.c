
#include "OpenACC/openacc.h"
#include "OpenACC/private/region.h"
#include "OpenACC/internal/region.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <stdlib.h>

#include <assert.h>

#ifndef DBG_REGION
#define DBG_REGION 0
#endif

typedef struct acc_region_desc_t_ * acc_region_desc_t;
typedef struct acc_region_t_ * acc_region_t;

struct acc_region_t_ * acc_build_region(size_t region_id) {
  acc_init_once();

  acc_region_desc_t region_desc = acc_region_desc_by_ID(region_id);
  assert(region_desc != NULL);

  unsigned num_devices = region_desc->num_devices;

  assert(num_devices > 0); /// \todo case when all present devices are used.

  struct acc_region_t_ * region = (struct acc_region_t_ *)malloc(sizeof(struct acc_region_t_) + num_devices * sizeof(struct acc_region_per_device_t_));

  region->desc = region_desc;

  region->param_ptrs = malloc(region_desc->num_params * sizeof(void *));

  region->scalar_ptrs = malloc(region_desc->num_scalars * sizeof(void *));

  region->data_ptrs = malloc(region_desc->num_datas * sizeof(d_void *));
  region->data_size = malloc(region_desc->num_datas * sizeof(size_t));

  region->loops = malloc(region_desc->num_loops * sizeof(struct acc_loop_t_));

  region->distributed_data = (h_void **)malloc(region_desc->num_distributed_data * (sizeof(h_void *) + sizeof(size_t)));

  region->num_devices = num_devices;

  if (region_desc->devices == NULL) {
    assert(num_devices == 1);

    acc_device_t dev_type = acc_get_device_type();
    int dev_num = acc_get_device_num(dev_type);

    region->devices[0].device_idx = acc_get_device_idx(dev_type, dev_num);
    region->devices[0].num_gang[0] = 0;
    region->devices[0].num_gang[1] = 0;
    region->devices[0].num_gang[2] = 0;
    region->devices[0].num_worker[0] = 0;
    region->devices[0].num_worker[1] = 0;
    region->devices[0].num_worker[2] = 0;
    region->devices[0].vector_length = 0;
  }
  else {
    unsigned i;
    for (i = 0; i < num_devices; i++) {
      region->devices[i].device_idx = acc_get_device_idx(region_desc->devices[i].kind, region_desc->devices[i].num);
      region->devices[i].num_gang[0] = 0;
      region->devices[i].num_gang[1] = 0;
      region->devices[i].num_gang[2] = 0;
      region->devices[i].num_worker[0] = 0;
      region->devices[i].num_worker[1] = 0;
      region->devices[i].num_worker[2] = 0;
      region->devices[i].vector_length = 0;
    }
  }

  return region;
}

void acc_region_execute(acc_region_t region) {
#if DBG_REGION
  printf("[debug]  acc_region_execute #%zd\n", region->desc->id);
#endif

  assert(region->desc->num_kernel_groups);

}

void acc_region_start(acc_region_t region) {
#if DBG_REGION
  printf("[debug]  acc_region_start #%zd\n", region->desc->id);
#endif

  acc_region_init(region);

  acc_get_region_defaults(region);

  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue);
  }
}

void acc_region_stop(acc_region_t region) {
#if DBG_REGION
  printf("[debug]  acc_region_stop #%zd\n", region->desc->id);
#endif
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue);
  }
}

