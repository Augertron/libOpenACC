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

struct acc_data_allocations_t_ {
  /*! \todo An OpenCL allocator manages the memory of one OpenCL device.
   *        It maintains a mapping of OpenACC host pointers to OpenCL buffers.
   *        It manages separately the device (host accessible) address spaces (RW, RO, WO).
   *        It knows the characteristics and usage of each memory.
   */
};
extern struct acc_data_allocations_t_ data_allocations;

d_void * acc_get_deviceptr(unsigned device_idx, h_void * host_ptr);

h_void * acc_get_hostptr(unsigned device_idx, d_void * device_ptr);

int acc_check_present(unsigned device_idx, h_void * host_ptr, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_ALLOCATOR */

/** @} */

