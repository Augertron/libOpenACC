/*!
 * \addtogroup grp_libopenacc_api_h2d Host to Device Communication
 * @{
 * 
 * \file libopenacc-host-to-device.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 *
 */

#ifndef __LIB_OPENACC_HOST_TO_DEVICE_RUNTIME_H__
#define __LIB_OPENACC_HOST_TO_DEVICE_RUNTIME_H__

typedef struct acc_loop_bounds_t_ {
  long lower;
  long upper;
  int stride;
} acc_loop_bounds_t;

/*! An iteration domain of size n is distriduted on 7 levels of tiles (7D tiles):
 *      - 3 of this levels correspond to work sharing accross gang, worker, and vector
 *          + gang and worker levels are merged to form the OpenCL global work size
 *          + vector are written in the inner most loop during code generation
 *      - the 4 remaining levels are placed in the kernel, between the transition
 */
typedef struct acc_loop_distribution_t_ {
  /// 
  unsigned sequential_iterations[4];
  enum { e_gang, e_worker, e_vector, e_none } shared_iterations[3];
} * acc_loop_distribution_t;

/*!
 *  This structure build by the host and transfered to the device's constant memory. 
 *  A new instance is created for each invocation of a kernel.
 *  It contains information from the parallel region and structural infornation about the loops.
 */
typedef struct acc_context_t_ {
   /// Number of Gang associated with the current parallel construct
   unsigned long num_gang;
   /// Number of Worker associated with the current parallel construct
   unsigned long num_worker;
   /// Number of Vector associated with the current parallel construct
   unsigned long vector_length;

   /// Number of loops in this kernel
   unsigned num_loop;
   /// Description of each loop in the kernel. Non-sized array: produced by over allocating an acc_context_t_ accordingly to num_loop
   struct {
     struct acc_loop_distribution_t_ distribution;
     struct acc_loop_bounds_t_ bounds;
   } loops[];
} * acc_context_t;

#endif /* __LIB_OPENACC_HOST_TO_DEVICE_RUNTIME_H__ */ /** @} */

