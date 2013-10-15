/*!
 * \addtogroup grp_libopenacc_impl_host Host Side
 * @{
 * 
 * \file lib-openacc.c
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "OpenACC/libopenacc-internal.h"

#include <assert.h>

#define NULL 0

// OpenACC public API

int acc_get_num_devices(acc_device_t dev) {
  assert("NIY"); /// \todo
  return 0;
}

void acc_set_device_type(acc_device_t dev) {
  assert("NIY"); /// \todo
}

acc_device_t acc_get_device_type() {
  assert("NIY"); /// \todo
  return NULL;
}

void acc_set_device_num (int id, acc_device_t dev) {
  assert("NIY"); /// \todo
}

int acc_get_device_num(acc_device_t dev) {
  assert("NIY"); /// \todo
  return -1;
}

int acc_async_test(int id) {
  assert("NIY"); /// \todo
  return -1;
}

int acc_async_test_all() {
  assert("NIY"); /// \todo
  return -1;
}

void acc_async_wait(int id) {
  assert("NIY"); /// \todo
}

void acc_async_wait_async(int id_wait, int id_async) {
  assert("NIY"); /// \todo
}

void acc_async_wait_all() {
  assert("NIY"); /// \todo
}

void acc_async_wait_all_async(int id) {
  assert("NIY"); /// \todo
}

void acc_init(acc_device_t dev) {
  assert("NIY"); /// \todo
}

void acc_shutdown(acc_device_t dev) {
  assert("NIY"); /// \todo
}

int acc_on_device(acc_device_t dev) {
  assert("NIY"); /// \todo
  return -1;
}

d_void * acc_malloc (size_t n) {
  assert("NIY"); /// \todo
  return NULL;
}

void acc_free(d_void * dev_ptr) {
  assert("NIY"); /// \todo
}

void * acc_copyin(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
  return NULL;
}

void * acc_create(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
  return NULL;
}

void * acc_present_or_create(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
  return NULL;
}

void * acc_pcreate(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
  return NULL;
}

void acc_copyout(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
}

void acc_delete(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
}

void acc_update_device(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
}

void acc_update_self(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
}

void acc_map_data(h_void * host_ptr, d_void * dev_ptr, size_t n) {
  assert("NIY"); /// \todo
}

void acc_unmap_data(h_void * host_ptr) {
  assert("NIY"); /// \todo
}

d_void * acc_deviceptr(h_void * host_ptr) {
  assert("NIY"); /// \todo
  return NULL;
}

h_void * acc_hostptr(d_void * dev_ptr) {
  assert("NIY"); /// \todo
  return NULL;
}

int acc_is_present(h_void * host_ptr, size_t n) {
  assert("NIY"); /// \todo
  return -1;
}

void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes) {
  assert("NIY"); /// \todo
}

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes) {
  assert("NIY"); /// \todo
}

// OpenACC private API

void acc_fail_if_error(acc_error_t err) {
  assert("NIY"); /// \todo
}

acc_parallel_t acc_build_parallel(unsigned num_dims, unsigned long * num_gang, unsigned long * num_worker, unsigned num_vector) {
  assert("NIY"); /// \todo
  return NULL;
}

acc_error_t acc_parallel_start(acc_parallel_t region) {
  assert("NIY"); /// \todo
  return -1;
}

acc_error_t acc_parallel_stop (acc_parallel_t region) {
  assert("NIY"); /// \todo
  return -1;
}

acc_kernel_t acc_build_kernel(unsigned kernel_id) {
  assert("NIY"); /// \todo
  return NULL;
}

acc_error_t acc_enqueue_kernel(acc_parallel_t region, acc_kernel_t kernel) {
  assert("NIY"); /// \todo
  return -1;
}

// OpenACC internal API

acc_context_t acc_create_context(acc_parallel_t region, acc_kernel_t kernel) {
  return NULL; /// \todo 
}

/** @} */

