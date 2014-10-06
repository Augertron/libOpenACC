/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   OpenACC/public/async.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_PUBLIC_ASYNC
#define OPENACC_PUBLIC_ASYNC 20131031

#include "OpenACC/openacc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 */
int acc_async_test(int);

/*!
 */
int acc_async_test_all();

/*!
 */
void acc_async_wait(int);

/*!
 */
void acc_async_wait_async(int, int);

/*!
 */
void acc_async_wait_all();

/*!
 */
void acc_async_wait_all_async(int);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PUBLIC_ASYNC */

/*! @} */

