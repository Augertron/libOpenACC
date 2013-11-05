
#include "OpenACC/private/region.h"
#include "OpenACC/internal/region.h"
#include "OpenACC/private/runtime.h"

#include <stdlib.h>

#include <assert.h>

typedef struct acc_region_desc_t_ * acc_region_desc_t;
typedef struct acc_region_t_ * acc_region_t;

struct acc_region_t_ * acc_build_region(acc_region_desc_t region, size_t num_dims, size_t * num_gang, size_t * num_worker, size_t vector_length) {
  acc_init_once();

  struct acc_region_t_ * result = (struct acc_region_t_ *)malloc(sizeof(struct acc_region_t_));

  result->desc          = region;
  result->num_dims      = num_dims;
  result->num_gang      = num_gang;
  result->num_worker    = num_worker;
  result->vector_length = vector_length;

  return result;
}

void acc_region_start(acc_region_t region) {
  acc_init_once();

  acc_region_init(region->desc->id, acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_get_region_defaults(region, acc_runtime.curr_device_type);

  // Compute device index
  acc_device_t dev = acc_get_device_type();
  size_t num = acc_get_device_type(dev);
  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);
  assert(num >= 0 && num < num_devices);
  region->device_idx = first_device + num;

  /// \todo acc_region_start : what else?
}

void acc_region_stop(acc_region_t region) {
  acc_init_once();

  /// \todo acc_region_stop : ???
}

