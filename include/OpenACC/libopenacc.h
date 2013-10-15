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

// Errors

typedef int acc_error_t;
void acc_fail_if_error(acc_error_t);

// Pointer to Data-Structure types

typedef struct acc_runtime_t_  * acc_runtime_t;
typedef struct acc_parallel_t_ * acc_parallel_t;
typedef struct acc_kernel_t_   * acc_kernel_t;
typedef acc_kernel_t (* acc_kernel_builder_t) ();

/// Store global information needed by the runtime for the current application (singleton)
struct acc_runtime_t {
  /// Current device
  acc_device_t curr_device_type;
  unsigned     curr_device_id;

  /// Number of available devices for each type of device
  unsigned num_devices[acc_device_last];

  /// Number of OpenCL kernels
  unsigned num_kernel;

  /*!
   *  Pointers to kernel builders. Kernel builders are function pointer.
   *  Kernel builder are generated at compile time and use the internal API.
   */
  acc_kernel_builder_t * kernel_builders;
};

/// global runtime object for OpenACC.
extern acc_runtime_t acc_runtime;

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

// Loop Directive (Host side)

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

#endif /* __LIB_OPENACC_H__ */  /** @} */

