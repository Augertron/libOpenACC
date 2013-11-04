
#include "OpenACC/private/region.h"
#include "OpenACC/internal/region.h"
#include "OpenACC/private/runtime.h"

#include <stdlib.h>

typedef struct acc_region_desc_t_ * acc_region_desc_t;

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

void acc_region_start(struct acc_region_t_ * region) {
  acc_init_once();

  acc_region_init(region->desc->id, acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_get_region_defaults(region, acc_runtime.curr_device_type);

  /// \todo acc_region_start : what else?
}

void acc_region_stop(struct acc_region_t_ * region) {
  acc_init_once();

  /// \todo acc_region_stop : ???
}

