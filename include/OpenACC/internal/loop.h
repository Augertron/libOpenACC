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

typedef enum acc_loop_tile_e_ {
  e_tile_0 = 0,
  e_gang   = 1,
  e_tile_1 = 2,
  e_worker = 3,
  e_tile_2 = 4,
  e_vector = 5,
  e_tile_3 = 6
} acc_loop_tile_e;

struct acc_loop_t_ {
  unsigned long num_iterations[7];
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_INTERNAL_LOOP */

/** @} */

