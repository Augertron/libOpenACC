
#include "OpenACC/openacc.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/memory.h"

#include <stdio.h>

#include <assert.h>

d_void * acc_malloc(size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_malloc_(device_idx, n);
}

void acc_free(d_void * dev_ptr) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_free_(device_idx, dev_ptr);
}

////////////////////////////////////////////////////////////////////////

d_void * acc_copyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_copyin_(device_idx, host_ptr, n);
}

d_void * acc_present_or_copyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_present_or_copyin_(device_idx, host_ptr, n);
}

d_void * acc_pcopyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_pcopyin_(device_idx, host_ptr, n);
}

d_void * acc_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_create_(device_idx, host_ptr, n);
}

d_void * acc_present(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_present_(device_idx, host_ptr, n);
}

d_void * acc_present_or_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_present_or_create_(device_idx, host_ptr, n);
}

d_void * acc_pcreate(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_pcreate_(device_idx, host_ptr, n);
}

void acc_copyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_copyout_(device_idx, host_ptr, n);
}

d_void * acc_present_or_copyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_present_or_copyout_(device_idx, host_ptr, n);
}

d_void * acc_pcopyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_pcopyout_(device_idx, host_ptr, n);
}

void acc_delete(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_delete_(device_idx, host_ptr, n);
}

////////////////////////////////////////////////////////////////////////

void acc_update_device(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_update_device_(device_idx, host_ptr, n);
}

void acc_update_self(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_update_self_(device_idx, host_ptr, n);
}

////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////

d_void * acc_deviceptr(h_void * host_ptr) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_deviceptr_(device_idx, host_ptr);
}

h_void * acc_hostptr(d_void * dev_ptr) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_hostptr_(device_idx, dev_ptr);
}

////////////////////////////////////////////////////////////////////////

int acc_is_present(h_void * host_ptr, size_t n) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  return acc_is_present_(device_idx, host_ptr, n);
}

////////////////////////////////////////////////////////////////////////

void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_memcpy_to_device_(device_idx, dest, src, bytes);
}

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_memcpy_from_device_(device_idx, dest, src, bytes);
}

