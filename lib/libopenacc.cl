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

#include "OpenACC/libopenacc-device.h"

size_t acc_gang_id(__constant struct acc_context_t_ * ctx) {
  return get_group_id(0);
}

size_t acc_gang_size(__constant struct acc_context_t_ * ctx) {
  return get_num_groups(0);
}

size_t acc_worker_id(__constant struct acc_context_t_ * ctx) {
  return get_global_id(0);
}

size_t acc_worker_size(__constant struct acc_context_t_ * ctx) {
  return get_local_size(0);
}

int acc_WS_mode(__constant struct acc_context_t_ * ctx) {
  return acc_worker_id(ctx) == 0;
}
/*
acc_loop_bounds_t acc_loop_bounds(__constant struct acc_context_t_ * ctx, unsigned loop_id, unsigned tile_id) {
  // ctx->loops[loop_id].distribution.tile[tile_level];
  // ctx->loops[loop_id].bounds;

  return ctx->loops[loop_id].tiles.bounds[tile_id];
}

long acc_loop_iteration(
  __constant struct acc_context_t_ * ctx,
  unsigned loop_id,
  unsigned long it_tile_0,
  unsigned long it_tile_1,
  unsigned long it_tile_2,
  unsigned long it_tile_3
) {
  long result = 0;

  // Outter tile
  result = (result + it_tile_0) * ctx->loops[loop_id].tiles.sizes[0];
  // Gang tile
  result = (result + acc_gang_id(ctx)) * acc_gang_size(ctx);
  // Tile between Gang and Worker
  result = (result + it_tile_1) * ctx->loops[loop_id].tiles.sizes[1];
  // Worker tile
  result = (result + acc_worker_id(ctx)) * acc_worker_size(ctx);
  // Tile between Worker and Vector
  result = (result + it_tile_2) * ctx->loops[loop_id].tiles.sizes[2];
  // Vector tile (vector iterator is 0 as the vector is hard coded by the code generator)
  result = (result + 0) * ctx->vector_length;
  // Inner tile
  result = (result + it_tile_3) * ctx->loops[loop_id].tiles.sizes[3];

  result += ctx->loops[loop_id].bounds.lower;

  return result;
}

int acc_loop_valid(
  __constant struct acc_context_t_ * ctx,
  unsigned loop_id,
  long it
) {
  return it >= ctx->loops[loop_id].bounds.lower && it < ctx->loops[loop_id].bounds.upper;
}
*/
