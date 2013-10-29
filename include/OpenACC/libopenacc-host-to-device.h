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

/*!
 *  This structure build by the host and transfered to the device's constant memory. 
 *  A new instance is created for each invocation of a kernel.
 *  It contains information from the parallel region and structural infornation about the loops.
 *
 *  \todo only support 1d worksize
 *  \todo only support increasing loops (however // loops can be reversed)
 */
typedef struct acc_context_t_ {
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
   struct {
     unsigned long strides[4];
     long lower;
     long upper;
   } loops[];
} * acc_context_t;

#endif /* __LIB_OPENACC_HOST_TO_DEVICE_RUNTIME_H__ */ /** @} */

