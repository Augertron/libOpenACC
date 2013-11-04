/*!
 * \addtogroup grp_libopenacc_api_private Private
 * @{
 * 
 * \file   OpenACC/private/init.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_PRIVATE_INIT
#define OPENACC_PRIVATE_INIT 20131031

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  \brief Initilized OpenACC Runtime. Called first by all function from the Public API.
 */
void acc_init_once();

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL */

/** @} */

