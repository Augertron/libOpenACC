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

#include "OpenACC/device/openacc.cl"

//volatile __global long acc_terminated_gangs = 0;

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

long acc_gang_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl) {
  return it_tile + acc_gang_id(ctx, lvl) * acc_get_tile_stride(ctx, tile_id);
}

long acc_worker_iteration(__constant struct acc_context_t_ * ctx, size_t tile_id, long it_tile, short lvl) {
  return it_tile + acc_worker_id(ctx, lvl) * acc_get_tile_stride(ctx, tile_id);
}

long acc_get_loop_lower(__constant struct acc_context_t_ * ctx, size_t loop_id) {
  return ctx->data[2 * loop_id];
}

long acc_get_loop_upper(__constant struct acc_context_t_ * ctx, size_t loop_id) {
  return ctx->data[2 * loop_id + 1];
}

long acc_get_tile_length(__constant struct acc_context_t_ * ctx, size_t tile_id) {
  return ctx->data[2 * ctx->num_loops + 2 * tile_id];
}

long acc_get_tile_stride(__constant struct acc_context_t_ * ctx, size_t tile_id) {
  return ctx->data[2 * ctx->num_loops + 2 * tile_id + 1];
}

int acc_is_master_gang_lvl(__constant struct acc_context_t_ * ctx, short lvl) {
  return acc_gang_id(ctx, lvl) == 0;
}

int acc_is_master_gang(__constant struct acc_context_t_ * ctx) {
  return acc_is_master_gang_lvl(ctx, 0)
      && acc_is_master_gang_lvl(ctx, 1)
      && acc_is_master_gang_lvl(ctx, 2);
}

int acc_terminate_gangs(__constant struct acc_context_t_ * ctx) {
  return 0;
  //return (atomic_inc(&acc_terminated_gangs)) == ctx->num_gangs[0] * ctx->num_gangs[1] * ctx->num_gangs[2]);
}

int acc_is_master_worker_lvl(__constant struct acc_context_t_ * ctx, short lvl) {
  return acc_worker_id(ctx, lvl) == 0;
}

int acc_is_master_worker(__constant struct acc_context_t_ * ctx) {
  return acc_is_master_worker_lvl(ctx, 0)
      && acc_is_master_worker_lvl(ctx, 1)
      && acc_is_master_worker_lvl(ctx, 2);
}

int acc_barrier_workers(__constant struct acc_context_t_ * ctx) {
  barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
  return acc_is_master_worker(ctx);
}



