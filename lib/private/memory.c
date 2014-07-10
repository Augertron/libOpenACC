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

#include "OpenACC/utils/profiling.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef DBG_DATA
# define DBG_DATA 0
#endif

#ifndef DBG_DIST_DATA
# define DBG_DIST_DATA 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

void acc_distributed_data(struct acc_region_t_ * region, size_t device_idx_, h_void ** host_ptr, size_t * n) {
  // Search corresponding "distributed data" entry in region descriptor
  size_t data_idx;
  for (data_idx = 0; data_idx < region->desc->num_distributed_data; data_idx++)
    if (region->data[region->desc->distributed_data[data_idx].id].ptr == *host_ptr)
      break;
  if (data_idx == region->desc->num_distributed_data) return; // No entry found: NOP (data goes entirely to all devices)

  // Check that found "distributed data" entry is valid
  assert( region->desc->distributed_data[data_idx].nbr_dev == region->desc->num_devices &&
          region->desc->distributed_data[data_idx].portions != NULL
        );

  // Search corresponding "device" entry in region descriptor 
  size_t device_idx;
  for (device_idx = 0; device_idx < region->desc->num_devices; device_idx++)
    if (region->devices[device_idx].device_idx == device_idx_)
      break;
  assert(device_idx < region->desc->num_devices);

  // Only support contiguous distribution mode at this point (chunk mode to be implemented)
  assert(region->desc->distributed_data[data_idx].mode == e_contiguous);

  // Compute sum of all portions and offset of this device portion (prev_portion)
  unsigned sum_portions = 0;
  unsigned prev_portion = 0;
  size_t k;
  for (k = 0; k < region->desc->num_devices; k++) {
    sum_portions += region->desc->distributed_data[data_idx].portions[k];
    if (k < device_idx)
      prev_portion += region->desc->distributed_data[data_idx].portions[k];
  }
#if DBG_DIST_DATA
  printf("[debug]   region[%u] on device #%u distributed data.\n", region->desc->id, device_idx_);
  printf("[debug]       host_ptr     = %x\n", *host_ptr);
  printf("[debug]       n            = %d\n", *n);
  printf("[debug]       sum_portions = %d\n", sum_portions);
  printf("[debug]       prev_portion = %d\n", prev_portion);
#endif
  // Update host_ptr and n to cover the desired portion
  *host_ptr += (*n * prev_portion) / sum_portions;
  *n         = (*n * region->desc->distributed_data[data_idx].portions[device_idx]) / sum_portions;

#if DBG_DIST_DATA
  printf("[debug]       host_ptr     = %x\n", *host_ptr);
  printf("[debug]       n            = %d\n", *n);
#endif
}

d_void * acc_malloc_(size_t device_idx, size_t n) {
#if DBG_DATA
  printf("[debug] acc_malloc_(size_t device_idx = %u, size_t n = %d)\n", device_idx, n);
#endif

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
    printf("[fatal]   clCreateBuffer return %s for device %zd and size %u.\n", status_str, device_idx, (unsigned)n);
    exit(-1); /// \todo error code
  }

  clFinish(acc_runtime.opencl_data->devices_data[device_idx]->command_queue);

#if DBG_DATA
  printf("[debug]     return %x\n", buffer);
#endif

  return (d_void *)buffer;
}

void acc_free_(size_t device_idx, d_void * dev_ptr) {
#if DBG_DATA
  printf("[debug] acc_free_(device_idx = %zd, dev_ptr = %x)\n", device_idx, dev_ptr);
#endif
  cl_int status = clReleaseMemObject((cl_mem)dev_ptr);
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clReleaseMemObject return %s for device ptr = %lx.\n", status_str, (unsigned long)dev_ptr);
    exit(-1); /// \todo error code
  }
}

////////////////////////////////////////////////////////////////////////

d_void * acc_copyin_(size_t device_idx, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_copyin_(device_idx = %zd, host_ptr = %x, n = %zd)\n", device_idx, host_ptr, n);
#endif

  assert(!acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_malloc_(device_idx, n);

  acc_map_data_(device_idx, host_ptr, dev_ptr, n);

  acc_memcpy_to_device_(device_idx, dev_ptr, host_ptr, n);

  return dev_ptr;
}

void acc_copyin_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_copyin_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
      acc_copyin_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_present_or_copyin_(size_t device_idx, h_void * host_ptr, size_t n) {
  if (acc_is_present_(device_idx, host_ptr, n))
    return acc_deviceptr_(device_idx, host_ptr);
  else
    return acc_copyin_(device_idx, host_ptr, n);
}

void acc_present_or_copyin_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_present_or_copyin_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
      acc_present_or_copyin_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_pcopyin_(size_t device_idx, h_void * host_ptr, size_t n) {
  return acc_present_or_copyin_(device_idx, host_ptr, n);
}

d_void * acc_create_(size_t device_idx, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_create_(device_idx = %zd, host_ptr = %x, n = %zd)\n", device_idx, host_ptr, n);
#endif
  assert(!acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_malloc_(device_idx, n);

  acc_map_data_(device_idx, host_ptr, dev_ptr, n);

  return dev_ptr;
}

void acc_create_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_create_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
      acc_create_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_present_(size_t device_idx, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_present_(device_idx = %zd, host_ptr = %x, n = %zd)\n", device_idx, host_ptr, n);
#endif
  assert(acc_is_present_(device_idx, host_ptr, n));
}

void acc_present_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_present_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
      acc_present_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_present_or_create_(size_t device_idx, h_void * host_ptr, size_t n) {
  if (acc_is_present_(device_idx, host_ptr, n))
    return acc_deviceptr_(device_idx, host_ptr);
  else
    return acc_create_(device_idx, host_ptr, n);
}

void acc_present_or_create_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_present_or_create_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif
  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
      acc_present_or_create_(region->devices[idx].device_idx, host_ptr_, n_);
  }
}

d_void * acc_pcreate_(size_t device_idx, h_void * host_ptr, size_t n) {
  return acc_present_or_create_(device_idx, host_ptr, n);
}

void acc_copyout_(size_t device_idx, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_copyout_(device_idx = %zd, host_ptr = %x, n = %zd)\n", device_idx, host_ptr, n);
#endif

  assert(acc_is_present_(device_idx, host_ptr, n));

  d_void * dev_ptr = acc_deviceptr_(device_idx, host_ptr);

  acc_memcpy_from_device_(device_idx, host_ptr, dev_ptr, n);
}

void acc_copyout_regions_(struct acc_region_t_ * region, h_void * host_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_copyout_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif

  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
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
#if DBG_DATA
  printf("[debug] acc_present_or_copyout_regions_(region = #%u, h_void * host_ptr = %x, size_t n = %d)\n", region->desc->id, host_ptr, n);
#endif

  acc_init_region_(region);
  unsigned idx;
  for (idx = 0; idx < region->desc->num_devices; idx++) {
    h_void * host_ptr_ = host_ptr;
    size_t n_ = n;
    acc_distributed_data(region, region->devices[idx].device_idx, &host_ptr_, &n_);
    if (n_ > 0)
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
#if DBG_DATA
  printf("[debug] acc_deviceptr_(device_idx = %zd, host_ptr = %x)\n", device_idx, host_ptr);
#endif

  d_void * dev_ptr = acc_get_deviceptr(device_idx, host_ptr);

  return dev_ptr;
}

h_void * acc_hostptr_(size_t device_idx, d_void * dev_ptr) {
#if DBG_DATA
  printf("[debug] acc_hostptr_(device_idx = %zd, dev_ptr = %x)\n", device_idx, dev_ptr);
#endif

  h_void * host_ptr = acc_get_hostptr(device_idx, dev_ptr);

  return host_ptr;
}

////////////////////////////////////////////////////////////////////////

int acc_is_present_(size_t device_idx, h_void * host_ptr, size_t n) {;
  return acc_check_present(device_idx, host_ptr, n);
}

////////////////////////////////////////////////////////////////////////

void acc_memcpy_to_device_(size_t device_idx, d_void * dest, h_void * src, size_t bytes) {
#if DBG_DATA
  printf("[debug] acc_memcpy_to_device_(device_idx = %zd, dev_ptr = %lx, host_ptr = %lx, bytes = %zd)\n", device_idx, (unsigned long)dest, (unsigned long)src, bytes);
#endif

  cl_event event;

  cl_int status = clEnqueueWriteBuffer(
    /* cl_command_queue command_queue  */ acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
    /* cl_mem buffer                   */ (cl_mem)dest,
    /* cl_bool blocking_write          */ CL_TRUE,
    /* size_t offset                   */ 0,
    /* size_t cb                       */ bytes,
    /* const void *ptr                 */ src,
    /* cl_uint num_events_in_wait_list */ 0,
    /* const cl_event *event_wait_list */ NULL,
    /* cl_event *event                 */ &event
  );
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clEnqueueWriteBuffer return %s for host ptr = %lx to device ptr = %lx of size %zd.\n", status_str, (unsigned long)src, (unsigned long)dest, bytes);
    exit(-1); /// \todo error code
  }

  acc_profiling_register_memcpy_to_device(event, device_idx, dest, src, bytes);
}

void acc_memcpy_from_device_(size_t device_idx, h_void * dest, d_void * src, size_t bytes) {
#if DBG_DATA
  printf("[debug] acc_memcpy_from_device_(device_idx = %zd, host_ptr = %lx, dev_ptr = %lx, bytes = %zd)\n", device_idx, (unsigned long)dest, (unsigned long)src, bytes);
#endif

  cl_event event;

  cl_int status = clEnqueueReadBuffer (
    /* cl_command_queue command_queue */ acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
    /* cl_mem buffer */ (cl_mem)src,
    /* cl_bool blocking_read */ CL_TRUE,
    /* size_t offset */ 0,
    /* size_t cb */ bytes,
    /* void *ptr */ dest,
    /* cl_uint num_events_in_wait_list */ 0,
    /* const cl_event *event_wait_list */ NULL,
    /* cl_event *event */ &event
  );
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal] clEnqueueReadBuffer return %s for device ptr = %lx to host ptr = %lx of size %zd.\n", status_str, (unsigned long)src, (unsigned long)dest, bytes);
    exit(-1); /// \todo error code
  }

  acc_profiling_register_memcpy_from_device(event, device_idx, dest, src, bytes);
}

#ifdef __cplusplus
}
#endif

/*! @} */

