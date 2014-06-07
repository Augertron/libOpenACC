/*!
 * \addtogroup grp_libopenacc_api_private
 * @{
 * 
 * \file OpenACC/private/region.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_PRIVATE_REGION
#define LIBOPENACC_PRIVATE_REGION 20131031

#include "OpenACC/openacc.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// A parallel region
struct acc_region_t_ {
  /// Pointer to region descriptor
  struct acc_region_desc_t_ * desc;

  /// param pointer
  void ** param_ptrs;

  /// scalar pointer
  void ** scalar_ptrs;

  /// data arguments, pointers to device memory
  d_void ** data_ptrs;
  size_t  * data_size;

  /// Loop bounds and stride: provided by transformed application
  struct acc_loop_t_ * loops;

  struct {
    h_void * ptr;
    size_t size;
  } * distributed_data;

  unsigned num_devices;

  struct acc_region_per_device_t_ {
    // Device Index
    size_t device_idx;
    /// Number of gang
    size_t num_gang[3];
    /// Number of worker
    size_t num_worker[3];
    /// Vector size used for this parallel region (needed to determine workers chunk size)
    size_t vector_length;
  } devices [];
};

struct acc_region_t_ * acc_build_region(size_t region_id);

void acc_region_execute(struct acc_region_t_ * region);

void acc_region_start(struct acc_region_t_ * region);
void acc_region_stop (struct acc_region_t_ * region);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_REGION */

/** @} */

