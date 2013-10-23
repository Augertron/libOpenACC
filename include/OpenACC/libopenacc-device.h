/*!
 * \addtogroup grp_libopenacc_api_device Device (OpenCL C)
 * @{
 * 
 * \file libopenacc-device.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 *
 */

#ifndef __LIB_OPENACC_DEVICE_H__
#define __LIB_OPENACC_DEVICE_H__

#define _OPENACC_LIB_DEVICE         1
#define _OPENACC_LIB_DEVICE_NAME    "OpenACC Device Runtime in OpenCL C for ROSE Compiler"
#define _OPENACC_LIB_DEVICE_VERSION 201310

#include "OpenACC/libopenacc-host-to-device.h"

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \return true if the current thread should run in Gang Partitionned mode 
 */
int acc_GP_mode(__constant struct acc_context_t_ * ctx);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \return true if the current thread should run in Worker Partitionned mode 
 */
int acc_WP_mode(__constant struct acc_context_t_ * ctx);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \return true if the current thread should run in Vector Partitionned mode 
 */
int acc_VP_mode(__constant struct acc_context_t_ * ctx);


/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel. 
 *  \param[loop_id] a unique loop_id identiying the loop in 'ctx'
 *  \param[tile_level] each loop in a parallel region is (virtually) tiled in 7 levels. 3 are used for Gang, Worker, and Vector. \
                       The remaining 4 can be left with iterations to execute.
 *  \return true if the current thread should run in Vector Partitionned mode 
 */
acc_loop_bounds_t acc_loop_bounds(__constant struct acc_context_t_ * ctx, unsigned loop_id, unsigned tile_level);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \param[loop_id] a unique loop_id identiying the loop in 'ctx'
 *  \param[tile_level] each loop in a parallel region is (virtually) tiled in 7 levels. 3 are used for Gang, Worker, and Vector. \
                       The remaining 4 can be left with iterations to execute.
 *  \param[i] current iteration of the tile 
 *  \return true if the current thread should run in Vector Partitionned mode 
 */
unsigned long acc_loop_iteration(__constant struct acc_context_t_ * ctx, unsigned loop_id, unsigned long tile_0, unsigned long tile_1, unsigned long tile_2, unsigned long tile_3);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \param[loop_id] a unique loop_id identiying the loop in 'ctx'
 *  \param[tile_level] each loop in a parallel region is (virtually) tiled in 7 levels. 3 are used for Gang, Worker, and Vector. \
                       The remaining 4 can be left with iterations to execute.
 *  \return true if the loop 'loop_id' have is not tiled at level 'tile_level'
 */
int acc_loop_no_tile(__constant struct acc_context_t_ * ctx, unsigned loop_id, unsigned tile_level);

#endif /* __LIB_OPENACC_DEVICE_H__ */ /** }@ */

