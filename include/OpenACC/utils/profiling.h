
#ifndef __LIBOPENACC_PROFILING_H__
#define __LIBOPENACC_PROFILING_H__

#include "OpenACC/openacc.h"

#include "sqlite3.h"
#include "OpenACC/utils/containers/map.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_profiler_t_ {
  char * db_file_name;

  size_t run_id;

  sqlite3 * db_file;

  map_t events;
};
typedef struct acc_profiler_t_ * acc_profiler_t;
extern acc_profiler_t acc_profiler;

/// Build the acc_profiler data-structure
acc_profiler_t acc_profiler_build_profiler();

/// Initialize OpenACC profiler
void acc_profiling_init();

/// Exit OpenACC profiler
void acc_profiling_exit();

void acc_profiling_release_all_events();

/// Create (if needed) 'Runs' table. DB query: "CREATE TABLE Runs ( 'user_field' )"
int acc_profiling_set_experiment(char * user_fields);

/// Create a new entry in 'Runs' table. DB query: "INSERT into Runs ( 'user_data' )". Update acc_profiler->run_id accordingly.
void acc_profiling_new_run(char * user_data);

/// Called to register a callback on 'memcpy to device' event
void acc_profiling_register_memcpy_to_device(cl_event event, size_t device_idx, h_void * host_ptr, d_void * dev_ptr, size_t size);

/// Called to register a callback on 'memcpy from device' event
void acc_profiling_register_memcpy_from_device(cl_event event, size_t device_idx, h_void * host_ptr, d_void * dev_ptr, size_t size);

/// Called to register a callback on 'kernel launch' event
void acc_profiling_register_kernel_launch(cl_event event, size_t device_idx, size_t region_id, size_t kernel_id);

#ifdef __cplusplus
}
#endif


#endif /* __LIBOPENACC_PROFILING_H__ */

