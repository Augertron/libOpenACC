/*!
 * \addtogroup grp_libopenacc_api_private
 * @{
 * 
 * \file OpenACC/private/runtime.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_PRIVATE_RUNTIME
#define LIBOPENACC_PRIVATE_RUNTIME 20131031

#include "OpenACC/public/arch.h"

#include <CL/cl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct acc_device_data_t_ {
  /// OpenCL Context for one device
  cl_context context;
  /// OpenCL Programs for one device for each parallel region
  cl_program * programs;
  // OpenCL Command Queue associated with this device
  cl_command_queue command_queue;
};
typedef struct acc_device_data_t_ * acc_device_data_t;

/// OpenACC Runtime OpenCL datas (singleton embedded into acc_runtime_t_)
typedef struct acc_opencl_data_t_ {
  /// Status of the last OpenCL call. 
  cl_int status;

  /// Number of platforms
  cl_uint num_platforms;

  /// Platform IDs ( cl_platform_id[num_platforms] )
  cl_platform_id * platforms;

  /// Number of devices per platform ( cl_uint[num_platforms + 1] , last is sum )
  cl_uint * num_devices;

  /// Device IDs ( cl_device_id[num_platforms][num_devices[platform_id]] allocated as cl_device_id[num_devices[num_platforms]] )
  cl_device_id ** devices;

  acc_device_data_t * devices_data;

  char * runtime_sources;
  char ** region_sources;

} * acc_opencl_data_t;

enum flags_e {
  f_alloc         = 0x0001,

  f_acc_defaults  = 0x0002,
  f_acc_devices   = 0x0004,

  f_ocl_devices   = 0x0008,
  f_ocl_sources   = 0x000F
};

/// OpenACC runtime global data (singleton)
typedef struct acc_runtime_t_ {
  uint32_t check_list;

  acc_opencl_data_t opencl_data;

  /// Each type of device is associated to a set of OpenCL devices
  struct {
    unsigned first;
    size_t num;
  } devices[acc_device_last];

  /// Current device type
  acc_device_t curr_device_type;

  /// Current device number
  int curr_device_num;

} acc_runtime_t;
/// Forward declaration of the unique instance of the singleton acc_runtime_t
extern acc_runtime_t acc_runtime;

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_RUNTIME */

/** @} */

