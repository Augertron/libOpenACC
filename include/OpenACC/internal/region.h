/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file   OpenACC/internal/region.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_INTERNAL_REGION
#define OPENACC_INTERNAL_REGION 20131031

#include "OpenACC/public/arch.h"
#include "OpenACC/private/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct acc_kernel_desc_t_ * acc_kernel_desc_t;

struct acc_region_desc_t_ {
  size_t id;

  char * file;

  size_t num_options;
  char ** options;

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

  /// number of private date arguments
  size_t num_privates;

  size_t num_loops;

  size_t num_kernel_groups;
  struct acc_kernel_group_t_ {
    size_t num_kernels;
    struct acc_kernel_with_deps_t_ {
      acc_kernel_desc_t kernel;
      size_t num_dependencies;
      size_t * dependencies;
    } * kernels;
  } * kernel_groups;

  size_t num_devices;

  size_t num_distributed_data;
  struct acc_data_distribution_t_ {
    size_t id;
    acc_splitting_mode_e mode;

    size_t nbr_dev;
    size_t * portions;

    size_t chunk;
  } * distributed_data;

  size_t num_splitted_loops;
  struct acc_loop_splitter_t_ {
    size_t id;

    acc_splitting_mode_e mode;

    size_t nbr_dev;
    size_t * portions;

    size_t chunk;
  } * splitted_loops;
};

struct acc_region_desc_t_ * acc_region_desc_by_ID(size_t region_id);

/**
 *  Initialize a region : build the associated OpenCL program (only have to be done once)
 */
void acc_region_init(struct acc_region_t_ * region);

/*!
 *  Set number gangs, number workers, and vector length to the current device default if they were not provided (value set to 0) 
 */
void acc_get_region_defaults(struct acc_region_t_ * region);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_REGION */

/** @} */

