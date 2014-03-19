/*!
 * \addtogroup grp_libopenacc_api_private
 * @{
 * 
 * \file OpenACC/private/kernel.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_PRIVATE_KERNEL
#define LIBOPENACC_PRIVATE_KERNEL 20131031

#include "OpenACC/public/def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct acc_loop_desc_t_ * acc_loop_desc_t;

struct acc_kernel_t_ {
  /// Pointer to kernel descriptor
  struct acc_kernel_desc_t_ * desc;

  /// param pointer
  void ** param_ptrs;

  /// scalar pointer
  void ** scalar_ptrs;

  /// data arguments, pointers to device memory
  d_void ** data_ptrs;
  size_t  * data_size;

  /// Loops descriptor for dynamically defined bounds and strides
  acc_loop_desc_t * loops;
};

/*! \func acc_build_kernel
 *
 *  Call a function pointer indexed on kernel id. 
 *
 *  \param id Unique ID of the kernel (global, not region specific)
 *  \return an OpenACC kernel descriptor, argument arrays are allocated but not initialized (need to be done before enqueuing the kernel)
 */
struct acc_kernel_t_ * acc_build_kernel(struct acc_kernel_desc_t_ * kernel);

/*! \func acc_enqueue_kernel
 *
 *  It enqueue 'kernel' in the queue associated with 'region'
 *
 *  \param region the current region
 *  \param kernel the kernel to launch
 *  \return a non-zero value if an error occured
 */
void acc_enqueue_kernel(struct acc_region_t_ * region, struct acc_kernel_t_ * kernel);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_KERNEL */

/** @} */

