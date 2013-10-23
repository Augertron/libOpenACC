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

typedef struct acc_region_desc_t_ * acc_region_desc_t;
typedef struct acc_region_t_      * acc_region_t;
typedef struct acc_kernel_desc_t_ * acc_kernel_desc_t;
typedef struct acc_kernel_t_      * acc_kernel_t;

struct acc_region_data_t_ {
};
typedef struct acc_region_data_t_ * acc_region_data_t;

struct acc_device_data_t_ {
  /// OpenCL Context for one device
  cl_context context;
  /// OpenCL Programs for one device for each parallel region
  cl_program * programs;
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

/// OpenACC runtime global data (singleton)
typedef struct acc_runtime_t_ {
  enum flags_e {
    f_alloc         = 0x0001,

    f_acc_defaults  = 0x0002,
    f_acc_devices   = 0x0004,

    f_ocl_devices   = 0x0008,
    f_ocl_sources   = 0x000F,
    f_ocl_kernels   = 0x0010
  };
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

/*!
 *  \brief Initilized OpenACC Runtime. Called first by all function from the Public API.
 */
void acc_init_once();

// ********************
// * Parallel regions *
// ********************

/// A parallel region
struct acc_region_t_ {
  size_t id;

  /// Number of dimension in the parallel region (currently only 1 supported by OpenACC directives)
  size_t num_dims;
  /// Number of gang   for each dimension
  size_t * num_gang;
  /// Number of worker for each dimension
  size_t * num_worker;

  /// Vector size used for this parallel region (needed to determine workers chunk size)
  size_t vector_length;
};

acc_region_t acc_build_region(size_t id , size_t num_dims, size_t * num_gangs, size_t * num_workers, size_t vector_length);

/*! \func acc_region_start
 *  \param region pointer to a parallel region descriptor
 *  \return a non-zero value if an error occured
 */
acc_error_t acc_region_start(acc_region_t region);

/*! \func acc_region_stop
 *  \param region pointer to a parallel region descriptor
 *  \return a non-zero value if an error occured
 */
acc_error_t acc_region_stop (acc_region_t region);

// **********************
// * Kernel Abstraction *
// **********************

struct acc_kernel_t_ {
  /// The kernel ID to retrieve the implementation (cl_kernel) from the runtime
  size_t id;

  /// scalar pointer
  void ** scalar_ptrs;

  /// data arguments, pointers to device memory
  d_void ** data_ptrs;
};

/*! \func acc_build_kernel
 *
 *  Call a function pointer indexed on kernel id. 
 *
 *  \param id Unique ID of the kernel (global, not region specific)
 *  \return an OpenACC kernel descriptor, argument arrays are allocated but not initialized (need to be done before enqueuing the kernel)
 */
acc_kernel_t acc_build_kernel(size_t id);

/*! \func acc_enqueue_kernel
 *
 *  It enqueue 'kernel' in the queue associated with 'region'
 *
 *  \param region the current region
 *  \param kernel the kernel to launch
 *  \return a non-zero value if an error occured
 */
acc_error_t acc_enqueue_kernel(acc_region_t region, acc_kernel_t kernel);

// *********************************
// * Debugging Interface (Printer) *
// *********************************

/*!
 * \brief Debug: dump OpenACC runtime information
 */
void acc_dbg_dump_runtime();

#endif /* __LIB_OPENACC_H__ */  /** @} */

