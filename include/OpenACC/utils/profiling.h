
#ifndef __LIBOPENACC_PROFILING_H__
#define __LIBOPENACC_PROFILING_H__

#include "OpenACC/openacc.h"
#include <stdbool.h>

/// An event descriptor
struct acc_profiling_event_data_t_ {
  /// Kind of event issued
  enum acc_profiling_event_kind_e {
    e_acc_memcpy_to_device,
    e_acc_memcpy_from_device,
    e_acc_kernel_launch
  } kind;

  /// Device for which to which the event is associated
  size_t device_idx;

  /// Data associated with the event
  union {
    /// Event is a kernel launch
    struct {
      size_t region_id;
      size_t kernel_id;
    } kernel_launch;
    /// Event is a memory transfert
    struct {
      h_void * host_ptr;
      d_void * dev_ptr;
      size_t size;
    } memcpy;
  } data;
};

/// Callback function called when a registered OpenCL event complete
void acc_profiling_ocl_event_callback(cl_event event, cl_int event_command_exec_status, void * user_data);

/// Initialize OpenACC profiler
void acc_profiling_init();

/// Exit OpenACC profiler
void acc_profiling_exit();

/// Create (if needed) 'Runs' table. DB query: "CREATE TABLE Runs ( 'user_field' )"
void acc_profiling_set_experiment(char * user_fields);

/// Create a new entry in 'Runs' table. DB query: "INSERT into Runs ( 'user_data' )". Update acc_profiler->run_id accordingly.
void acc_profiling_new_run(char * user_data);


#endif /* __LIBOPENACC_PROFILING_H__ */

