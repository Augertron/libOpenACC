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

enum acc_opencl_address_spaces_e_ {
  e_ocl_rw_address_space,
  e_ocl_ro_address_space,
  e_ocl_wo_address_space,
  e_ocl_address_space_any
};
typedef enum acc_opencl_address_spaces_e_ acc_opencl_address_spaces_e;

struct acc_opencl_data_alloc_t_ {
  struct acc_opencl_data_alloc_t_ * parent;

  h_void * host_ptr;
  size_t size;
  acc_opencl_address_spaces_e address_space;
  
  unsigned dev_idx;
  d_void * dev_ptr;
};

struct acc_opencl_allocator_t_ {
  /*! \todo An OpenCL allocator manages the memory of one OpenCL device.
   *        It maintains a mapping of OpenACC host pointers to OpenCL buffers.
   *        It manages separately the device (host accessible) address spaces (RW, RO, WO).
   *        It knows the characteristics and usage of each memory.
   */
};
extern struct acc_opencl_allocator_t_ opencl_allocator;

/*!
 *  \brief Build an allocation descriptor for a given host_ptr on a given device (allocation can already exist)
 */
struct acc_opencl_data_alloc_t_ * acc_alloc_(unsigned device_idx, h_void * host_ptr, size_t size, acc_opencl_address_spaces_e address_space);

/*!
 *  \brief Release an allocation
 *
 *  \post free(data_alloc)
 */
void acc_free_(struct acc_opencl_data_alloc_t_ * data_alloc);

d_void * acc_get_deviceptr(unsigned device_idx, h_void * host_ptr);

h_void * acc_get_hostptr(unsigned device_idx, d_void * device_ptr);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_ALLOCATOR */

/** @} */

