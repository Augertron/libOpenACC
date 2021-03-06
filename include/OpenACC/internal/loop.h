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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum acc_splitting_mode_e_ {
  e_contiguous,
  e_chunk
} acc_splitting_mode_e;

enum acc_tile_kind_e {
  e_gang_tile = 0,
  e_worker_tile = 1,
  e_vector_tile = 2,
  e_dynamic_tile = 3,
  e_static_tile = 4,
  e_unrolled_tile = 5
};

struct acc_tile_desc_t_ {
  size_t id;
  enum acc_tile_kind_e kind;
  union {
    size_t nbr_it; // valid iff kind = e_static_tile or e_unrolled_tile
    size_t lvl;    // valid iff kind = e_gang_tile or e_worker_tile,
  } param;
};

struct acc_loop_desc_t_ {
  size_t id;
  size_t num_tiles;
  struct acc_tile_desc_t_ * tiles;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_LOOP */

/** @} */

