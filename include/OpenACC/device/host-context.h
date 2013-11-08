/*!
 * \addtogroup grp_libopenacc_api_h2d Host to Device Communication
 * @{
 * 
 * \file OpenACC/device/host-context.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_INTERNAL_H2D
#define LIBOPENACC_INTERNAL_H2D 20131031

#include "OpenACC/internal/loop.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_kernel_loop_t_ {
  struct acc_loop_desc_t_ original;
  struct {
    long stride;
    long length; /// Negative length for negative stride
  } tiles[7];
};

/*!
 *  This structure build by the host and transfered to the device's constant memory. 
 *  A new instance is created for each invocation of a kernel.
 *  It contains information from the parallel region and structural infornation about the loops.
 *
 *  \note See http://enja.org/2011/03/30/adventures-in-opencl-part-3-constant-memory-structs/                                                                                                                  
 *
 *  \todo only support 1d worksize
 *  \todo only support increasing loops (however // loops can be reversed)
 */
struct acc_context_t_ {
   /// Number of Gang associated with the current parallel construct. Unused: number of gangs == number of workgroups.
   unsigned long num_gang;
   /// Number of Worker associated with the current parallel construct. Unused: number of gangs == number of workgroups.
   unsigned long num_worker;
   /// Number of Vector associated with the current parallel construct.
   unsigned long vector_length;

   /// Number of loops in this kernel. Unused: loop accesses are hard coded by kernel generator.
   unsigned num_loop;

   /*! An iteration domain of size n is distriduted on 7 levels of tiles (7D tiles):
    *      - 3 of this levels correspond to work sharing accross gang, worker, and vector
    *          + gang and worker levels are merged to form the OpenCL global work size
    *          + vector are written in the inner most loop during code generation
    *      - the 4 remaining levels are placed in the kernel surrounding the vector
    *  Inclusive bounds
    */
   struct acc_kernel_loop_t_ loops[];
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_H2D */

/** @} */

