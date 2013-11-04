/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file OpenACC/internal/loop.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_INTERNAL_LOOP
#define LIBOPENACC_INTERNAL_LOOP 20131031

#include "OpenACC/private/loop.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_loop_tile_t_ {
  enum {
    e_tiling_dynamic,          ///!< Tile is dynamic, neither the number of iterations or the stride have been defined by the code generator.
    e_tiling_static_iteration, ///!< Tile is static,  the number of iterations has been defined by the code generator. The stride has to be compute when launching the kernel.
    e_tiling_static_stride,    ///!< Tile is static,  stride has been defined by the code generator. The number of iterations has to be compute when launching the kernel.
    e_tiling_static_size,      ///!< Tile is static,  number of iterations and stride have been defined by the code generator.
    e_no_tiling
  } tiling;

  long stride;
  unsigned long iteration;
};

enum acc_loop_tile_e {
  e_tile_0 = 0,
  e_gang   = 1,
  e_tile_1 = 2,
  e_worker = 3,
  e_tile_2 = 4,
  e_vector = 5,
  e_tile_3 = 6
};

struct acc_loop_t_ {
  struct acc_loop_tile_t_ tiles[7];
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_LOOP */

/** @} */

