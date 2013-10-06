/*!
 * \file lib-openacc-device.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 * 
 * Device Runtime for OpenCL C.
 *
 */

#ifndef __LIB_OPENACC_DEVICE_H__
#define __LIB_OPENACC_DEVICE_H__

//#include "OpenACC/lib-openacc-host-to-device.h"
typedef struct acc_context_t_ * acc_context_t;
typedef struct acc_bounds_t_ * acc_bounds_t;

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \return true if the current thread should run in Gang Partitionned mode 
 */
int acc_GP_mode(__constant acc_context_t ctx);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \return true if the current thread should run in Worker Partitionned mode 
 */
int acc_WP_mode(__constant acc_context_t ctx);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \return true if the current thread should run in Vector Partitionned mode 
 */
int acc_VP_mode(__constant acc_context_t ctx);


/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel. 
 *  \param[loop_id] a unique loop_id identiying the loop in 'ctx'
 *  \param[tile_level] each loop in a parallel region is (virtually) tiled in 7 levels. 3 are used for Gang, Worker, and Vector. \
                       The remaining 4 can be left with iterations to execute.
 *  \return true if the current thread should run in Vector Partitionned mode 
 */
acc_bounds_t acc_loop_bounds(__constant acc_context_t ctx, unsigned loop_id, unsigned tile_level);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \param[loop_id] a unique loop_id identiying the loop in 'ctx'
 *  \param[tile_level] each loop in a parallel region is (virtually) tiled in 7 levels. 3 are used for Gang, Worker, and Vector. \
                       The remaining 4 can be left with iterations to execute.
 *  \param[i] current iteration of the tile 
 *  \return true if the current thread should run in Vector Partitionned mode 
 */
unsigned long acc_loop_iteration(__constant acc_context_t ctx, unsigned loop_id, unsigned tile_level, unsigned long i);

/*!
 *  \param[ctx] an OpenACC Context stored in constant memory. It is provided by the host when calling the kernel.
 *  \param[loop_id] a unique loop_id identiying the loop in 'ctx'
 *  \param[tile_level] each loop in a parallel region is (virtually) tiled in 7 levels. 3 are used for Gang, Worker, and Vector. \
                       The remaining 4 can be left with iterations to execute.
 *  \return true if the loop 'loop_id' have is not tiled at level 'tile_level'
 */
int acc_loop_no_tile(__constant acc_context_t ctx, unsigned loop_id, unsigned tile_level);

#endif /* __LIB_OPENACC_DEVICE_H__ */

