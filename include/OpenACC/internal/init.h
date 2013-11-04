/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file   OpenACC/internal/init.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_INTERNAL_INIT
#define OPENACC_INTERNAL_INIT 20131031

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int check_flag(uint32_t flag);
void set_flag(uint32_t flag);

/*!
 *  \brief Retrieved information from OS environment variables.
 */
void acc_init_defaults();

/*!
 *  \brief Sorts collected Devices
 */
void acc_init_devices();

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL */

/** @} */

