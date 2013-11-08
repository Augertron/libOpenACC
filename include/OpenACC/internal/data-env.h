/*!
 * \addtogroup grp_libopenacc_api_internal
 * @{
 * 
 * \file OpenACC/internal/data-env.h
 *
 * \author Tristan Vanderbruggen
 * \date 11/07/2013
 *
 */

#ifndef LIBOPENACC_INTERNAL_DATA_ENV
#define LIBOPENACC_INTERNAL_DATA_ENV 20131031

#include "OpenACC/private/data-env.h"
#include "OpenACC/public/def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct acc_opencl_data_alloc_t_ * acc_opencl_data_alloc_t;

struct acc_data_environment_t_ {
  struct acc_data_environment_t_ * parent;
  struct acc_data_environment_t_ * child;

  /// \todo fields: host to device ptrs map, one per device...
};
extern struct acc_data_environment_t_ * data_environment;

struct acc_data_environment_t_ * acc_build_data_environment(struct acc_data_environment_t_ * parent);

void acc_map_data_(unsigned device_idx, h_void * host_ptr, d_void * dev_ptr, size_t n);

void acc_unmap_data_(unsigned device_idx, h_void * host_ptr);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_DATA_ENV */

/** @} */

