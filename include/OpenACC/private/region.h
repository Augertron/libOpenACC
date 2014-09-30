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

  /// data arguments
  struct acc_data_t_ {
    h_void * ptr;
    size_t nbr_elements;
    size_t element_size;
    size_t dominant_dimension; /// currently always 0 (1st dimension), used to distribute data between multiple accelerators.
    size_t nbr_elements_dominant_dimension;
  } * data;

  /// private data arguments
  struct acc_private_t_ {
    h_void * ptr;
    size_t nbr_elements;
    size_t element_size;
  } * privates;

  /// Loop bounds and stride: provided by transformed application
  struct acc_loop_t_ * loops;

  struct acc_region_per_device_t_ {
    size_t device_idx;
    size_t num_gang[3];
    size_t num_worker[3];
    size_t vector_length;
  } * devices;
};

struct acc_region_t_ * acc_region_build(size_t region_id);

void acc_region_execute(struct acc_region_t_ * region);

void acc_region_start(struct acc_region_t_ * region);
void acc_region_stop (struct acc_region_t_ * region);

void acc_region_free(struct acc_region_t_ * region);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_REGION */

/** @} */

