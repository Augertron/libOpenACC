/*!
 * \addtogroup grp_libopenacc_api_internal
 * @{
 * 
 * \file OpenACC/internal/allocator.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_INTERNAL_ALLOCATOR
#define LIBOPENACC_INTERNAL_ALLOCATOR 20131031

#include "OpenACC/public/def.h"
#include "OpenACC/public/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_data_allocation_t_ {
  h_void * host_ptr;
  d_void * dev_ptr;
  size_t size;
};

//

void init_memory_manager();

void free_memory_manager();

//

void register_data_allocation(unsigned device_idx, const struct acc_data_allocation_t_ * data_alloc);

void remove_data_allocation(unsigned device_idx, const h_void * data_alloc);

d_void * acc_get_deviceptr(unsigned device_idx, const h_void * host_ptr);

h_void * acc_get_hostptr(unsigned device_idx, const d_void * device_ptr);

int acc_check_present(unsigned device_idx, const h_void * host_ptr, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_ALLOCATOR */

/** @} */

