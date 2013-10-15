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

struct acc_loop_bounds_t {
  long lower;
  long upper;
  int stride;
};

/*! An iteration domain of size n is distriduted on 7 levels of tiles (7D tiles):
 *      - 3 of this levels correspond to work sharing accross gang, worker, and vector
 *          + gang and worker levels are merged to form the OpenCL global work size
 *          + vector are written in the inner most loop during code generation
 *      - the 4 remaining levels are placed in the kernel, between the transition
 */
struct acc_loop_distribution_t {
  /// 
  unsigned sequential_iterations[4];
  enum { e_gang, e_worker, e_vector, e_none } shared_iterations[3];
};

/*!
 *  This structure build by the host and transfered to the device's constant memory. 
 *  A new instance is created for each invocation of a kernel.
 *  It contains information from the parallel region and structural infornation about the loops.
 */
struct acc_context_t_ {
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
     acc_loop_distribution_t distribution;
     acc_loop_bounds_t bounds;
   } loops[];
};
typedef acc_context_t_ * acc_context_t;

#endif /* __LIB_OPENACC_HOST_TO_DEVICE_RUNTIME_H__ */ /** @} */

