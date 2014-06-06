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

#include "OpenACC/device/host-context.h"

long acc_gang_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl);
long acc_worker_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl);

size_t acc_gang_id(__constant struct acc_context_t_ * ctx, short lvl);
size_t acc_worker_id(__constant struct acc_context_t_ * ctx, short lvl);

size_t acc_gang_size(__constant struct acc_context_t_ * ctx, short lvl);
size_t acc_worker_size(__constant struct acc_context_t_ * ctx, short lvl);

#endif /* __LIB_OPENACC_DEVICE_H__ */ /** }@ */

