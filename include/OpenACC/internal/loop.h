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

enum acc_tile_kind_e {
  e_gang_tile,
  e_worker_tile,
  e_vector_tile,
  e_dynamic_tile,
  e_static_tile,
  e_unrolled_tile
};

struct acc_tile_desc_t_ {
  enum acc_tile_kind_e kind;
  union param {
    size_t nbr_it; // valid iff kind = e_static_tile or e_unrolled_tile
    size_t level;  // valid iff kind = e_gang_tile or e_worker_tile,
  };
  size_t original_loop_id;
};

struct acc_loop_desc_t_ {
  size_t num_tiles;
  size_t * tiles;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_LOOP */

/** @} */

