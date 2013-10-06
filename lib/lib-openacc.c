/*!
 * \file lib-openacc.c
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 * 
 * This file contains the code generated for "OpenACC to OpenCL Compiler" by "MDCG OpenCL Wrapper" for the file openacc.c
 * 
 * The naming convention used for this file should be followed by the compiler. It will help debugging and change tracking
 *
 * There is 3 part in this OpenACC API: 
 *     - public   : Can be used by OpenACC users, completely defined by OpenACC 2.0 standard
 *     - private  : To be used by the generated code ONLY. Not accessible to user.
 *     - internal : Used by functions of the public or private API
 *
 */

#include "OpenACC/openacc.h"
#include "OpenACC/lib-openacc.h"
#include "OpenACC/lib-openacc-internal.h"

#include <cassert>

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

int acc_async_test(int async) {
  assert("NIY"); /// \todo
  return -1;
}

int acc_async_test_all() {
  assert("NIY"); /// \todo
  return -1;
}

void acc_async_wait(int) {
  assert("NIY"); /// \todo
}

void acc_async_wait_async(int , int) {
  assert("NIY"); /// \todo
}

void acc_async_wait_all() {
  assert("NIY"); /// \todo
}

void acc_async_wait_all_async(int) {
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

d_void * acc_malloc (size_t) {
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

void fail_if_error(acc_error_t err) {
  /// \todo
}

// OpenACC internal API

acc_context_t acc_create_context(acc_parallel_region_t region, acc_kernel_t kernel) {
  return NULL; /// \todo 
}

