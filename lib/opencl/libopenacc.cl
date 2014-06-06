/*!
 * \addtogroup grp_libopenacc_impl_device Device Side
 * @{
 * 
 * \file lib-openacc.cl
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 *
 */

#include "OpenACC/device/opencl.h"

long acc_gang_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl) {
  return it_tile + acc_gang_id(ctx, lvl) * ctx->tiles[tile_id].stride;
}

long acc_worker_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl) {
  return it_tile + acc_worker_id(ctx, lvl) * ctx->tiles[tile_id].stride;
}

size_t acc_gang_id(__constant struct acc_context_t_ * ctx, short lvl) {
  return get_group_id(lvl);
}

size_t acc_gang_size(__constant struct acc_context_t_ * ctx, short lvl) {
  return get_num_groups(lvl);
}

size_t acc_worker_id(__constant struct acc_context_t_ * ctx, short lvl) {
  return get_local_id(lvl);
}

size_t acc_worker_size(__constant struct acc_context_t_ * ctx, short lvl) {
  return get_local_size(lvl);
}

