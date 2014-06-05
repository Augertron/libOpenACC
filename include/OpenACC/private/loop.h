/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file OpenACC/private/loop.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_PRIVATE_LOOP
#define LIBOPENACC_PRIVATE_LOOP 20131031

#ifdef __cplusplus
extern "C" {
#endif

/// Bounds are inclusives
struct acc_loop_t_ {
  long lower;
  long upper;
  long stride;
};

struct acc_tile_t_ {
  long length;
  long stride;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_PRIVATE_LOOP */

/** @} */

