/*!
 * \addtogroup grp_libopenacc_api_private Private
 * @{
 * 
 * \file libopenacc.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 *
 */

#ifndef __LIB_OPENACC_H__
#define __LIB_OPENACC_H__

#define _OPENACC_LIB         1
#define _OPENACC_LIB_NAME    "OpenACC for Rose Compiler"
#define _OPENACC_LIB_VERSION 201310

#include "OpenACC/openacc.h"

#include <CL/cl.h>

// Errors

typedef int acc_error_t;
void acc_fail_if_error(acc_error_t);

// Pointer to Data-Structure types

typedef struct acc_parallel_t_ * acc_parallel_t;
typedef struct acc_kernel_t_   * acc_kernel_t;

struct acc_device_data_t_ {
  cl_context context;
  cl_program program;
};
typedef struct acc_device_data_t_ * acc_device_data_t;

/// OpenACC Runtime OpenCL datas (singleton embedded into acc_runtime_t_)
typedef struct acc_runtime_ocl_t_ {
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

  size_t num_ocl_sources;
  char ** opencl_sources;

} * acc_runtime_ocl_t;

/// OpenACC runtime global data (singleton)
typedef struct acc_runtime_t_ {
  enum flags_e {
    f_alloc         = 0x0001,

    f_acc_defaults  = 0x0002,
    f_acc_devices   = 0x0004,

    f_ocl_devices   = 0x0008,
    f_ocl_sources   = 0x000F
  };
  uint32_t check_list;

  acc_runtime_ocl_t opencl_data;

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

/*!
 *  \brief Initilized OpenACC Runtime. Called first by all function from the Public API.
 */
void acc_init_once();

// ********************
// * Parallel regions *
// ********************

/// A parallel region descriptor
struct acc_parallel_region_t_ {
  /// Number of dimension in the parallel region (currently only 1 supported by OpenACC directives)
  unsigned num_dims;
  /// Number of gang   for each dimension
  unsigned long * num_gang;
  /// Number of worker for each dimension
  unsigned long * num_worker;

  /// Vector size used for this parallel region (needed to determine workers chunk size)
  unsigned vector_length;
};

acc_parallel_t acc_build_parallel(unsigned, unsigned long *, unsigned long *, unsigned);

/*! \func acc_parallel_start
 *  \param region pointer to a parallel region descriptor
 *  \return a non-zero value if an error occured
 */
acc_error_t acc_parallel_start(acc_parallel_t region);

/*! \func acc_parallel_stop
 *  \param region pointer to a parallel region descriptor
 *  \return a non-zero value if an error occured
 */
acc_error_t acc_parallel_stop (acc_parallel_t region);

// **********************
// * Kernel Abstraction *
// **********************

struct acc_kernel_t_ {
  /// The kernel ID to retrieve the implementation from the runtime
  unsigned kernel_id;

  /// scalar arguments
  void ** scalar_arguments;

  /// scalar arguments
  unsigned * scalar_sizes;

  /// number of scalar arguments
  unsigned num_scalar_arguments;

  /// data arguments, pointers to device memory
  d_void ** data_arguments;

  /// number of data arguments
  unsigned num_data_arguments;
};

/*! \func acc_build_kernel
 *
 *  Call a function pointer indexed on kernel id. 
 *
 *  \param kernel_id ID of the kernel to be launch
 *  \return an OpenACC kernel descriptor, argument arrays are allocated but not initialized (need to be done before enqueuing the kernel)
 */
acc_kernel_t acc_build_kernel(unsigned kernel_id);

/*! \func acc_enqueue_kernel
 *
 *  It enqueue 'kernel' in the queue associated with 'region'
 *
 *  \param region pointer to a parallel region descriptor
 *  \param kernel pointer to a kernel descriptor
 *  \return a non-zero value if an error occured
 */
acc_error_t acc_enqueue_kernel(acc_parallel_t region, acc_kernel_t kernel);

// *********************************
// * Debugging Interface (Printer) *
// *********************************

/*!
 * \brief Debug: dump OpenACC runtime information
 */
void acc_dbg_dump_runtime();

#endif /* __LIB_OPENACC_H__ */  /** @} */

