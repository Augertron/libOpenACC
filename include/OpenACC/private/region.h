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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// A parallel region
struct acc_region_t_ {
  /// Pointer to region descriptor
  struct acc_region_desc_t_ * desc;

  /// Number of dimension in the parallel region (currently only 1 supported by OpenACC directives)
  size_t num_dims;
  /// Number of gang   for each dimension
  size_t * num_gang;
  /// Number of worker for each dimension
  size_t * num_worker;

  /// Vector size used for this parallel region (needed to determine workers chunk size)
  size_t vector_length;
};

struct acc_region_t_ * acc_build_region(struct acc_region_desc_t_ * region, size_t num_dims, size_t * num_gangs, size_t * num_workers, size_t vector_length);

/*! \func acc_region_start
 *  \param region pointer to a parallel region descriptor
 *  \return a non-zero value if an error occured
 */
void acc_region_start(struct acc_region_t_ * region);

/*! \func acc_region_stop
 *  \param region pointer to a parallel region descriptor
 *  \return a non-zero value if an error occured
 */
void acc_region_stop (struct acc_region_t_ * region);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_REGION */

/** @} */

