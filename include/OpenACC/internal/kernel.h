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
  size_t id;

  /// Number of Gangs assumed by this version (0 means dynamic)
  size_t num_gang[3];

  /// Number of Workers assumed by this version (0 means dynamic)
  size_t num_worker[3];

  /// Vector length assumed by this version (0 means dynamic)
  size_t vector_length;

  /// Distributed loops (which are translated in tiles)
  struct acc_loop_desc_t_ * loops;

  /// Number of tiles (all loops include)
  size_t num_tiles;

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
  size_t  loop_id;

  acc_splitting_mode_e mode;

  size_t nbr_dev;
  size_t * portions;

  size_t  chunk;
};
typedef struct acc_loop_splitter_t_ * acc_loop_splitter_t;

struct acc_kernel_desc_t_ {
  size_t  id;

  /// Kernel name in the OpenCL C code
  char * name;

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

  size_t num_loops;

  unsigned num_versions;
  acc_kernel_version_t * versions;

  /// splitted loop, no splitted loop if NULL
  acc_loop_splitter_t splitted_loop;

  /// If not NULL: versions to use with the different devices (as listed by acc_region_desc_t::devices)
  size_t * version_by_devices; 
};

struct acc_kernel_desc_t_ * acc_kernel_desc_by_ID(size_t region_id, size_t kernel_id);

struct cl_kernel_ * acc_build_ocl_kernel(
  struct acc_region_t_ * region,
  struct acc_kernel_t_ * kernel,
  struct acc_context_t_ ** context,
  size_t device_idx
);

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_KERNEL */

/** @} */

