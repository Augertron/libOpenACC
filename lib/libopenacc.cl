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


int acc_GP_mode(__constant struct acc_context_t_ * ctx) {

  /// \todo acc_GP_mode

  return 0;
}

int acc_WP_mode(__constant struct acc_context_t_ * ctx) {

  /// \todo acc_WP_mode

  return 0;
}

int acc_VP_mode(__constant struct acc_context_t_ * ctx) {

  /// \todo acc_VP_mode

  return 0;
}

acc_loop_bounds_t acc_loop_bounds(__constant struct acc_context_t_ * ctx, unsigned loop_id, unsigned tile_level) {
  acc_loop_bounds_t result;

  /// \todo acc_loop_bounds

  return result;
}

int acc_loop_no_tile(__constant struct acc_context_t_ * ctx, unsigned loop_id, unsigned tile_level) {

  /// \todo acc_loop_no_tile

  return 0;
}

unsigned long acc_loop_iteration(
  __constant struct acc_context_t_ * ctx,
  unsigned loop_id,
  unsigned long tile_0,
  unsigned long tile_1,
  unsigned long tile_2,
  unsigned long tile_3
) {

  /// \todo acc_loop_iteration

  return 0;
}

