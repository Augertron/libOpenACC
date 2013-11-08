
#include "OpenACC/openacc.h"
#include "OpenACC/private/runtime.h"

#include <stdio.h>

#include <assert.h>

d_void * acc_malloc(size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  cl_int status;

  cl_mem buffer = clCreateBuffer(
    /* cl_context context  */ acc_runtime.opencl_data->devices_data[device_idx]->context,
    /* cl_mem_flags flags  */ CL_MEM_READ_WRITE,
    /* size_t size         */ n,
    /* void *host_ptr      */ NULL,
    /* cl_int *errcode_ret */ &status
  );
  if (status != CL_SUCCESS) {
    char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clCreateBuffer return %s for device %u and size %u.\n", status_str, device_idx, (unsigned)n);
    exit(-1); /// \todo error code
  }

  return (d_void *)buffer;
}

void acc_free(d_void * dev_ptr) {
  acc_init_once();

  cl_int status = clReleaseMemObject((cl_mem)dev_ptr);
  if (status != CL_SUCCESS) {
    char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clReleaseMemObject return %s for device ptr = %u.\n", status_str, dev_ptr);
    exit(-1); /// \todo error code
  }
}



d_void * acc_copyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!acc_is_present(host_ptr, n));

  d_void * dev_ptr = acc_malloc(n);

  acc_map_data(host_ptr, dev_ptr, n);

  acc_memcpy_to_device(dev_ptr, host_ptr, n);

  return dev_ptr;
}

d_void * acc_present_or_copyin(h_void * host_ptr, size_t n) {
  if (acc_is_present(host_ptr, n))
    return acc_deviceptr(host_ptr);
  else
    return acc_copyin(host_ptr, n);
}

d_void * acc_pcopyin(h_void * host_ptr, size_t n) {
  return acc_present_or_copyin(host_ptr, n);
}

d_void * acc_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!acc_is_present(host_ptr, n));

  d_void * dev_ptr = acc_malloc(n);

  acc_map_data(host_ptr, dev_ptr, n);

  return dev_ptr;
}

d_void * acc_present_or_create(h_void * host_ptr, size_t n) {
  if (acc_is_present(host_ptr, n))
    return acc_deviceptr(host_ptr);
  else
    return acc_create(host_ptr, n);
}

d_void * acc_pcreate(h_void * host_ptr, size_t n) {
  return acc_present_or_create(host_ptr, n);
}

void acc_copyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(acc_is_present(host_ptr, n));

  d_void * dev_ptr = acc_deviceptr(host_ptr);

  acc_memcpy_from_device(host_ptr, dev_ptr, n);
}

void acc_delete(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(acc_is_present(host_ptr, n));

  d_void * dev_ptr = acc_deviceptr(host_ptr);

  acc_unmap_data(host_ptr);

  acc_free(dev_ptr);
}



void acc_update_device(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(acc_is_present(host_ptr, n));

  d_void * dev_ptr = acc_deviceptr(host_ptr);

  acc_memcpy_to_device(dev_ptr, host_ptr, n);
}

void acc_update_self(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(acc_is_present(host_ptr, n));

  d_void * dev_ptr = acc_deviceptr(host_ptr);

  acc_memcpy_from_device(host_ptr, dev_ptr, n);
}



void acc_map_data(h_void * host_ptr, d_void * dev_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_map_data_(device_idx, host_ptr, dev_ptr, n);
}

void acc_unmap_data(h_void * host_ptr) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_unmap_data_(device_idx, host_ptr);
}



d_void * acc_deviceptr(h_void * host_ptr) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  d_void * dev_ptr = acc_get_deviceptr(device_idx, host_ptr);

  return dev_ptr;
}

h_void * acc_hostptr(d_void * dev_ptr) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  h_void * host_ptr = acc_get_hostptr(device_idx, dev_ptr);

  return host_ptr;
}



int acc_is_present(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_check_present(device_idx, host_ptr, n);
}


void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  cl_int status = clEnqueueWriteBuffer(
    /* cl_command_queue command_queue  */ acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
    /* cl_mem buffer                   */ (cl_mem)dest,
    /* cl_bool blocking_write          */ CL_FALSE,
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

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  cl_int status = clEnqueueReadBuffer (
    /* cl_command_queue command_queue  */ acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
    /* cl_mem buffer                   */ (cl_mem)src,
    /* cl_bool blocking_read           */ CL_FALSE,
    /* size_t offset                   */ 0,
    /* size_t cb                       */ bytes,
    /* void *ptr                       */ dest,
    /* cl_uint num_events_in_wait_list */ 0,
    /* const cl_event *event_wait_list */ NULL,
    /* cl_event *event                 */ NULL
  );
  if (status != CL_SUCCESS) {
    char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clEnqueueReadBuffer return %s for device ptr = %X to host ptr = %X of size %u.\n", status_str, (unsigned int)src, (unsigned int)dest, (unsigned int)bytes);
    exit(-1); /// \todo error code
  }
}

