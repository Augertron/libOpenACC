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

/*!
 *  This structure build by the host and transfered to the device's constant memory. 
 *  A new instance is created for each invocation of a kernel.
 *  It contains information from the parallel region and structural infornation about the loops.
 *
 *  \note See http://enja.org/2011/03/30/adventures-in-opencl-part-3-constant-memory-structs/                                                                                                                  
 *
 */
struct acc_context_t_ {
   size_t num_tiles; // (unused) struct cannot be empty when flexible size array is used
   struct acc_tile_t_ tiles[];
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_H2D */

/** @} */

