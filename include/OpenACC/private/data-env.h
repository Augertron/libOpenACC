/*!
 * \addtogroup grp_libopenacc_api_private
 * @{
 * 
 * \file OpenACC/private/data-env.h
 *
 * \author Tristan Vanderbruggen
 * \date 11/07/2013
 *
 */

#ifndef LIBOPENACC_PRIVATE_DATA_ENV
#define LIBOPENACC_PRIVATE_DATA_ENV 20131031

#include "OpenACC/public/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

void acc_push_data_environment();

void acc_pop_data_environment();

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_DATA_ENV */

/** @} */

