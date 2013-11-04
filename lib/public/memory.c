
#include "OpenACC/openacc.h"

#include <assert.h>

d_void * acc_malloc (size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void acc_free(d_void * dev_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void * acc_copyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void * acc_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void * acc_present_or_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void * acc_pcreate(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void acc_copyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_delete(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_update_device(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_update_self(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_map_data(h_void * host_ptr, d_void * dev_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_unmap_data(h_void * host_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

d_void * acc_deviceptr(h_void * host_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

h_void * acc_hostptr(d_void * dev_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

int acc_is_present(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

