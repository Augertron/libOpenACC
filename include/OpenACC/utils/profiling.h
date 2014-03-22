
#ifndef __LIBOPENACC_PROFILING_H__
#define __LIBOPENACC_PROFILING_H__

#include "OpenACC/openacc.h"

struct acc_profiling_event_data_t_ {
  enum acc_profiling_event_kind_e {
    e_acc_memcpy_to_device,
    e_acc_memcpy_from_device,
    e_acc_kernel_launch
  } kind;

  size_t device_idx;

  union {
    struct {
      size_t region_id;
      size_t kernel_id;
    } kernel_launch;
    struct {
      h_void * host_ptr;
      d_void * dev_ptr;
      size_t size;
    } memcpy;
  } data;
};

void acc_profiling_ocl_event_callback(cl_event event, cl_int event_command_exec_status, void * user_data);

void acc_profiling_init();

void acc_profiling_exit();

#endif /* __LIBOPENACC_PROFILING_H__ */

