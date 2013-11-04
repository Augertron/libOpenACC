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

