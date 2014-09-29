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

#include "OpenACC/private/loop.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  This structure build by the host and transfered to the device's constant memory. 
 *  A new instance is created for each invocation of a kernel.
 *  It contains information from the parallel region and structural infornation about the loops.
 *
 *  \note See http://enja.org/2011/03/30/adventures-in-opencl-part-3-constant-memory-structs/                                                                                                                  
 *
 *  data[2 * i]     for i < num_loops is loop #i lower bound
 *  data[2 * i + 1] for i < num_loops is loop #i upper bound
 *
 *  data[2 * num_loops + 2 * j]     for j < num_tiles is tile #j length
 *  data[2 * num_loops + 2 * j + 1] for j < num_tiles is tile #j stride
 */
struct acc_context_t_ {
   unsigned num_loops;
   unsigned num_tiles; // (unused) struct cannot be empty when flexible size array is used
   long num_gangs[3];
   long num_workers[3];
   long data[];
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_H2D */

/** @} */

