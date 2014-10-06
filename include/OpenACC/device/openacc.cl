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

#ifdef ROSE_LANGUAGE_MODE
#define __constant
#endif

#include "OpenACC/device/host-context.h"

size_t acc_gang_id(__constant struct acc_context_t_ * ctx, short lvl);
size_t acc_worker_id(__constant struct acc_context_t_ * ctx, short lvl);

size_t acc_gang_size(__constant struct acc_context_t_ * ctx, short lvl);
size_t acc_worker_size(__constant struct acc_context_t_ * ctx, short lvl);

long acc_gang_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl);
long acc_worker_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl);

long acc_get_loop_lower(__constant struct acc_context_t_ * ctx, size_t loop_id);
long acc_get_loop_upper(__constant struct acc_context_t_ * ctx, size_t loop_id);

long acc_get_tile_length(__constant struct acc_context_t_ * ctx, size_t tile_id);
long acc_get_tile_stride(__constant struct acc_context_t_ * ctx, size_t tile_id);

int acc_is_master_gang_lvl(__constant struct acc_context_t_ * ctx, short lvl);
int acc_is_master_gang(__constant struct acc_context_t_ * ctx);
int acc_terminate_gangs(__constant struct acc_context_t_ * ctx);

int acc_is_master_worker_lvl(__constant struct acc_context_t_ * ctx, short lvl);
int acc_is_master_worker(__constant struct acc_context_t_ * ctx);
int acc_barrier_workers(__constant struct acc_context_t_ * ctx);

#endif /* __LIB_OPENACC_DEVICE_H__ */ /** }@ */

