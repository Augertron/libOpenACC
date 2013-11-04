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

struct acc_loop_desc_t_ {
  long lower;
  long upper;
  long stride;
  unsigned long nbr_it;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_PRIVATE_LOOP */

/** @} */

