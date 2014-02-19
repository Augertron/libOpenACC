/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file OpenACC/internal/kernel.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_INTERNAL_KERNEL
#define LIBOPENACC_INTERNAL_KERNEL 20131031

#include "OpenACC/private/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/device/host-context.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Descriptor of one version of one kernel
 */
struct acc_kernel_version_t_ {
  /// Number of Gangs assumed by this version (0 means dynamic)
  unsigned long num_gang;

  /// Number of Workers assumed by this version (0 means dynamic)
  unsigned long num_worker;

  /// Vector length assumed by this version (0 means dynamic)
  unsigned long vector_length;

  /// Filling tiles for each loop
  struct acc_loop_t_ * loops;

  /// suffix added to the name of the kernel (suffixes have a predetermined form, but building it at runtime would be long)  
  char * suffix;
};
typedef struct acc_kernel_version_t_ * acc_kernel_version_t;

struct acc_kernel_desc_t_ {
  unsigned id;

  /// Kernel name in the OpenCL C code
  const char * name;

  /// number of scalar arguments
  size_t num_scalars;
  /// size of scalar arguments
  size_t * size_scalars;

  /// number of data arguments
  size_t num_datas;

  /// number of loops
  size_t num_loops;

  unsigned num_versions;
  acc_kernel_version_t * versions;

  unsigned num_splitted_loops;
  unsigned * splitted_loops;
};

/*! \func acc_create_context
 *
 *  Create the context associated to 'region' and 'kernel'
 *
 *  \param region pointer to a parallel region descriptor
 *  \param kernel pointer to a kernel descriptor
 *  \return a non-zero value if an error occured
 */
struct acc_context_t_ * acc_create_context(
  struct acc_region_t_ * region,
  struct acc_kernel_t_ * kernel,
  size_t device_idx
);

cl_kernel acc_build_ocl_kernel(
  struct acc_region_t_ * region,
  struct acc_kernel_t_ * kernel,
  struct acc_context_t_ * context,
  size_t device_idx
);

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_KERNEL */

/** @} */

