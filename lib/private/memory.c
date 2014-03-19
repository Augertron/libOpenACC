/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   lib/private/memory.c
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#include "OpenACC/openacc.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/memory.h"
#include "OpenACC/private/region.h"
#include "OpenACC/private/debug.h"
#include "OpenACC/internal/mem-manager.h"
#include "OpenACC/internal/region.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

void acc_distributed_data(struct acc_region_t_ * region, size_t device_idx, h_void ** host_ptr, size_t * n) {
  size_t i, j;
  for (i = 0; i < region->desc->num_distributed_data; i++)
    if (region->distributed_data[i] == host_ptr)
      break;
  if (i == region->desc->num_distributed_data) return;

  struct acc_data_distribution_t_ * data_distribution = &(region->desc->distributed_data[i]);

  assert( data_distribution->mode == e_contiguous &&
          data_distribution->nbr_dev == region->num_devices &&
          data_distribution->portions != NULL
        );

  for (i = 0; i < region->num_devices; i++)
    if (region->devices[i].device_idx == device_idx)
      break;
  assert(i < region->num_devices);

  unsigned sum_portions = 0;
  unsigned prev_portion = 0;
  for (j = 0; j < region->num_devices; j++) {
    sum_portions += data_distribution->portions[j];
    if (j < i)
      prev_portion += data_distribution->portions[j];
  }

  *host_ptr += (*n * prev_portion                  ) / sum_portions;
  *n         = (*n * data_distribution->portions[i]) / sum_portions;
}

d_void * acc_malloc_(size_t device_idx, size_t n) {
  cl_int status;
	
  cl_mem buffer = clCreateBuffer(
    /* cl_context context  */ acc_runtime.opencl_data->devices_data[device_idx]->context,
    /* cl_mem_flags flags  */ CL_MEM_READ_WRITE,
    /* size_t size         */ n,
    /* void *host_ptr      */ NULL,
    /* cl_int *errcode_ret */ &status
  );
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clCreateBuffer return %s for device %u and size %u.\n", status_str, device_idx, (unsigned)n);
    exit(-1); /// \todo error code
  }

//printf("acc_malloc(n = %u) = %x\n", n, buffer);

  return (d_void *)buffer;
}

void acc_free_(size_t device_idx, d_void * dev_ptr) {
  cl_int status = clReleaseMemObject((cl_mem)dev_ptr);
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clReleaseMemObject return %s for device ptr = %x.\n", status_str, dev_ptr);
    exit(-1); /// \todo error code
  }
}

////////////////////////////////////////////////////////////////////////

d_void * acc_copyin_(size_t device_idx, h_void * host_ptr, size_t n) {

  assert(!acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_malloc_(device_idx, n);

  acc_map_data_(device_idx, host_ptr, dev_ptr, n);

  acc_memcpy_to_device_(device_idx, dev_ptr, host_ptr, n);

  return dev_ptr;
}

void acc_copyin_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    acc_copyin_(region->devices[idx].device_idx, host_ptr, n);
  }
}

d_void * acc_present_or_copyin_(size_t device_idx, h_void * host_ptr, size_t n) {
  if (acc_is_present_(device_idx, host_ptr, n))
    return acc_deviceptr_(device_idx, host_ptr);
  else
    return acc_copyin_(device_idx, host_ptr, n);
}

void acc_present_or_copyin_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    acc_present_or_copyin_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_pcopyin_(size_t device_idx, h_void * host_ptr, size_t n) {
  return acc_present_or_copyin_(device_idx, host_ptr, n);
}

d_void * acc_create_(size_t device_idx, h_void * host_ptr, size_t n) {
  assert(!acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_malloc_(device_idx, n);

  acc_map_data_(device_idx, host_ptr, dev_ptr, n);

  return dev_ptr;
}

void acc_create_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    acc_create_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_present_or_create_(size_t device_idx, h_void * host_ptr, size_t n) {
  if (acc_is_present_(device_idx, host_ptr, n))
    return acc_deviceptr_(device_idx, host_ptr);
  else
    return acc_create_(device_idx, host_ptr, n);
}

void acc_present_or_create_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    acc_present_or_create_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_pcreate_(size_t device_idx, h_void * host_ptr, size_t n) {
  return acc_present_or_create_(device_idx, host_ptr, n);
}

void acc_copyout_(size_t device_idx, h_void * host_ptr, size_t n) {
  assert(acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_deviceptr_(device_idx, host_ptr);

  acc_memcpy_from_device_(device_idx, host_ptr, dev_ptr, n);
}

void acc_copyout_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    acc_copyout_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_present_or_copyout_(size_t device_idx, h_void * host_ptr, size_t n) {
  /// \todo incorrect need to check if {host_ptr, n} is allocated on top of the data-env
  if (acc_is_present_(device_idx, host_ptr, n))
    acc_copyout_(device_idx, host_ptr, n);
  return acc_deviceptr_(device_idx, host_ptr); /// should return NULL if previous executed
}

void acc_present_or_copyout_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    acc_present_or_copyout_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_pcopyout_(size_t device_idx, h_void * host_ptr, size_t n) {
  return acc_present_or_copyout_(device_idx, host_ptr, n);
}

void acc_delete_(size_t device_idx, h_void * host_ptr, size_t n) {
  assert(acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_deviceptr_(device_idx, host_ptr);

  acc_unmap_data_(device_idx, host_ptr);

  acc_free_(device_idx, dev_ptr);
}

////////////////////////////////////////////////////////////////////////

void acc_update_device_(size_t device_idx, h_void * host_ptr, size_t n) {
  assert(acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_deviceptr_(device_idx, host_ptr);

  acc_memcpy_to_device_(device_idx, dev_ptr, host_ptr, n);
}

void acc_update_self_(size_t device_idx, h_void * host_ptr, size_t n) {
  assert(acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_deviceptr_(device_idx, host_ptr);

  acc_memcpy_from_device_(device_idx, host_ptr, dev_ptr, n);
}

////////////////////////////////////////////////////////////////////////

d_void * acc_deviceptr_(size_t device_idx, h_void * host_ptr) {

  d_void * dev_ptr = acc_get_deviceptr(device_idx, host_ptr);

  return dev_ptr;
}

h_void * acc_hostptr_(size_t device_idx, d_void * dev_ptr) {

  h_void * host_ptr = acc_get_hostptr(device_idx, dev_ptr);

  return host_ptr;
}

////////////////////////////////////////////////////////////////////////

int acc_is_present_(size_t device_idx, h_void * host_ptr, size_t n) {;
  return acc_check_present(device_idx, host_ptr, n);
}

////////////////////////////////////////////////////////////////////////

void acc_memcpy_to_device_(size_t device_idx, d_void * dest, h_void * src, size_t bytes) {

  cl_int status = clEnqueueWriteBuffer(
    /* cl_command_queue command_queue  */ acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
    /* cl_mem buffer                   */ (cl_mem)dest,
    /* cl_bool blocking_write          */ CL_TRUE,
    /* size_t offset                   */ 0,
    /* size_t cb                       */ bytes,
    /* const void *ptr                 */ src,
    /* cl_uint num_events_in_wait_list */ 0,
    /* const cl_event *event_wait_list */ NULL,
    /* cl_event *event                 */ NULL
  );
  if (status != CL_SUCCESS) {
    char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clEnqueueWriteBuffer return %s for host ptr = %X to device ptr = %X of size %u.\n", status_str, (unsigned int)src, (unsigned int)dest, (unsigned int)bytes);
    exit(-1); /// \todo error code
  }
}

void acc_memcpy_from_device_(size_t device_idx, h_void * dest, d_void * src, size_t bytes) {

  cl_int status = clEnqueueReadBuffer (
    /* cl_command_queue command_queue */ acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
    /* cl_mem buffer */ (cl_mem)src,
    /* cl_bool blocking_read */ CL_TRUE,
    /* size_t offset */ 0,
    /* size_t cb */ bytes,
    /* void *ptr */ dest,
    /* cl_uint num_events_in_wait_list */ 0,
    /* const cl_event *event_wait_list */ NULL,
    /* cl_event *event */ NULL
  );
  if (status != CL_SUCCESS) {
    char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal] clEnqueueReadBuffer return %s for device ptr = %X to host ptr = %X of size %u.\n", status_str, (unsigned int)src, (unsigned int)dest, (unsigned int)bytes);
    exit(-1); /// \todo error code
  }
}

#ifdef __cplusplus
}
#endif

/*! @} */

