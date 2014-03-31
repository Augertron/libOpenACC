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

#include "OpenACC/openacc.h"
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

  /// suffix added to the name of the kernel 
  char * suffix;

  /// Device affinity, version will only be used on this device (default: acc_device_any)
  acc_device_t device_affinity;
};
typedef struct acc_kernel_version_t_ * acc_kernel_version_t;

typedef enum acc_splitting_mode_e_ {
  e_all,
  e_contiguous,
  e_chunk
} acc_splitting_mode_e;

/**
 *  Describe how one loop can be distributed accross devices.
 */
struct acc_loop_splitter_t_ {
  unsigned loop_id;

  acc_splitting_mode_e mode;

  unsigned nbr_dev;
  unsigned * portions;

  unsigned chunk;
};
typedef struct acc_loop_splitter_t_ * acc_loop_splitter_t;

struct acc_kernel_desc_t_ {
  unsigned id;

  /// Kernel name in the OpenCL C code
  const char * name;

  /// number of scalar arguments
  size_t num_params;
  /// size of scalar arguments
  size_t * size_params;

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

  /// splitted loop, no splitted loop if NULL
  acc_loop_splitter_t splitted_loop;

  /// If not NULL: versions to use with the different devices (as listed by acc_region_desc_t::devices)
  size_t * version_by_devices; 
};

/*! \func acc_create_context
 *
 *  Create the context associated to 'region' and 'kernel'
 *
 *  \param region pointer to a parallel region descriptor
 *  \param kernel pointer to a kernel descriptor
 *  \param current device being processed
 *  \return a non-zero value if an error occured
 */
struct acc_context_t_ * acc_create_context(
  struct acc_region_t_ * region,
  struct acc_kernel_t_ * kernel,
  size_t device_idx
);

struct cl_kernel_ * acc_build_ocl_kernel(
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

