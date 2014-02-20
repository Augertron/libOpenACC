/*!
 * \addtogroup grp_libopenacc_api_private Public
 * @{
 * 
 * \file   OpenACC/private/memory.h
 * \author Tristan Vanderbruggen
 * \date   02/18/2014
 *
 */

#ifndef OPENACC_PRIVATE_MEMORY
#define OPENACC_PRIVATE_MEMORY

#include "OpenACC/openacc.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_region_t_;

d_void * acc_malloc_(size_t device_idx, size_t);

void acc_free_(size_t device_idx, d_void *);

///////////////////////////////////////////////////////////////////////

d_void * acc_copyin_(size_t device_idx, h_void *, size_t);
void acc_copyin_regions_(struct acc_region_t_ * region, h_void *, size_t);

d_void * acc_present_or_copyin_(size_t device_idx, h_void *, size_t);
void acc_present_or_copyin_regions_(struct acc_region_t_ * region, h_void *, size_t);

d_void * acc_pcopyin_(size_t device_idx, h_void *, size_t);

d_void * acc_create_(size_t device_idx, h_void *, size_t);
void acc_create_regions_(struct acc_region_t_ * region, h_void *, size_t);

d_void * acc_present_or_create_(size_t device_idx, h_void *, size_t);
void acc_present_or_create_regions_(struct acc_region_t_ * region, h_void *, size_t);

d_void * acc_pcreate_(size_t device_idx, h_void *, size_t);

void acc_copyout_(size_t device_idx, h_void*, size_t);
void acc_copyout_regions_(struct acc_region_t_ * region, h_void*, size_t);

d_void * acc_present_or_copyout_(size_t device_idx, h_void *, size_t);
void acc_present_or_copyout_regions_(struct acc_region_t_ * region, h_void *, size_t);

d_void * acc_pcopyout_(size_t device_idx, h_void *, size_t);

void acc_delete_(size_t device_idx, h_void *, size_t);

///////////////////////////////////////////////////////////////////////

void acc_update_device_(size_t device_idx, h_void *, size_t);

void acc_update_self_(size_t device_idx, h_void *, size_t);

///////////////////////////////////////////////////////////////////////

// Implemented in internal/data-env.c
void acc_map_data_(size_t device_idx, h_void *, d_void *, size_t);

// Implemented in internal/data-env.c
void acc_unmap_data_(size_t device_idx, h_void *);

/// \todo Implement in internal/mem-manager.c
d_void * acc_deviceptr_(size_t device_idx, h_void *);

/// \todo Implement in internal/mem-manager.c
h_void * acc_hostptr_(size_t device_idx, d_void *);

/// \todo Implement in internal/mem-manager.c
int acc_is_present_(size_t device_idx, h_void *, size_t);

///////////////////////////////////////////////////////////////////////

void acc_memcpy_to_device_(size_t device_idx, d_void * dest, h_void * src, size_t bytes);

void acc_memcpy_from_device_(size_t device_idx, h_void * dest, d_void * src, size_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_MEMORY */

/*! @} */

