
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

typedef struct acc_kernel_t_ * acc_kernel_t;

struct acc_region_t_ * acc_region_build(size_t region_id) {
  acc_init_once();

  acc_region_desc_t region_desc = acc_region_desc_by_ID(region_id);
  assert(region_desc != NULL);

  unsigned num_devices = region_desc->num_devices;

  assert(num_devices > 0); /// \todo case when all present devices are used.

  struct acc_region_t_ * region = (struct acc_region_t_ *)malloc(sizeof(struct acc_region_t_) + num_devices * sizeof(struct acc_region_per_device_t_));

  region->desc = region_desc;

  region->param_ptrs = malloc(region_desc->num_params * sizeof(void *));

  region->scalar_ptrs = malloc(region_desc->num_scalars * sizeof(void *));

  region->data = malloc(region_desc->num_datas * sizeof(struct acc_data_t_));

  region->loops = malloc(region_desc->num_loops * sizeof(struct acc_loop_t_));

  region->devices = malloc(region_desc->num_devices * sizeof(struct acc_region_per_device_t_));

  if (region_desc->num_devices == 0) {
    assert(0); /// \todo all devices
  }
  else {
    size_t i;
    for (i = 0; i < num_devices; i++) {
      region->devices[i].device_idx = 0;
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

void acc_region_free(acc_region_t region) {
  free(region->param_ptrs);
  free(region->scalar_ptrs);
  free(region->data);
  free(region->loops);
  free(region->devices);
  free(region);
}

void acc_region_execute(acc_region_t region) {
#if DBG_REGION
  printf("[debug]  acc_region_execute #%zd\n", region->desc->id);
#endif

  assert(region->desc->num_kernel_groups == 1); // Only one version of the region is supported (need a criterium to select an implementation...)
  assert(region->desc->kernel_groups != NULL);

  assert(region->desc->kernel_groups[0].num_kernels == 1); // Only one kernel: if multiple dependencies could exist
  assert(region->desc->kernel_groups[0].kernels != NULL);
  assert(region->desc->kernel_groups[0].kernels[0].num_dependencies == 0); // No deps

  assert(region->desc->kernel_groups[0].kernels[0].kernel != NULL);

  size_t i;

  acc_region_start(region);

  acc_kernel_t kernel = acc_build_kernel(region->desc->kernel_groups[0].kernels[0].kernel);

  
  // Set parameter arguments
  for (i = 0; i < kernel->desc->num_params; i++)
    kernel->param_ptrs[i] = region->param_ptrs[kernel->desc->param_ids[i]];

  // Set scalar arguments
  for (i = 0; i < kernel->desc->num_scalars; i++)
    kernel->scalar_ptrs[i] = region->scalar_ptrs[kernel->desc->scalar_ids[i]];

  // Set data arguments
  for (i = 0; i < kernel->desc->num_datas; i++) {
    kernel->data_ptrs[i] = region->data[kernel->desc->data_ids[i]].ptr;
    kernel->data_size[i] = region->data[kernel->desc->data_ids[i]].nbr_elements * region->data[kernel->desc->data_ids[i]].element_size;
  }

  // Configure the loop
  for (i = 0; i < kernel->desc->num_loops; i++) {
    size_t loop_id = kernel->desc->loop_ids[i];
    kernel->loops[i].lower  = region->loops[loop_id].lower;
    kernel->loops[i].upper  = region->loops[loop_id].upper;
    kernel->loops[i].stride = region->loops[loop_id].stride;
  }

  acc_enqueue_kernel(region, kernel);

  acc_region_stop(region);

  free(kernel->param_ptrs);
  free(kernel->scalar_ptrs);
  free(kernel->data_ptrs);
  free(kernel->data_size);
  free(kernel->loops);
  free(kernel);
}

void acc_region_start(acc_region_t region) {
#if DBG_REGION
  printf("[debug]  acc_region_start #%zd\n", region->desc->id);
#endif

  acc_region_init(region);

  acc_get_region_defaults(region);

  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue);
  }
}

void acc_region_stop(acc_region_t region) {
#if DBG_REGION
  printf("[debug]  acc_region_stop #%zd\n", region->desc->id);
#endif
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    assert(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue != NULL);
    clFinish(acc_runtime.opencl_data->devices_data[region->devices[idx].device_idx]->command_queue);
  }
}

