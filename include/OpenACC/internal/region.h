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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct acc_kernel_desc_t_ * acc_kernel_desc_t;

struct acc_region_desc_t_ {
  unsigned id;

  char * file;

  size_t num_options;
  char ** options;

  size_t num_kernels;
  acc_kernel_desc_t * kernels;
};

/**
 *  Initialize a region : build the associated OpenCL program
 */
void acc_region_init(size_t region_id, acc_device_t dev, int num);

/*!
 *  Set number gangs, number workers, and vector length to the current device default if they were not provided (value set to 0) 
 */
void acc_get_region_defaults(struct acc_region_t_ * region, acc_device_t dev);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_REGION */

/** @} */

