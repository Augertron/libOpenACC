/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   OpenACC/public/runtime.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_PUBLIC_RUNTIME
#define OPENACC_PUBLIC_RUNTIME 20131031

#include "OpenACC/openacc.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_region_t_;

/*!
 *  \brief Initializes all devices of a given type
 *
 *  \param dev type of devices to initialize
 */
void acc_init(acc_device_t dev);

/*!
 *  \brief Initializes one device
 *
 *  \param dev type of the device
 *  \param num the device number
 */
void acc_init_(acc_device_t dev, int num);

/*!
 *  \brief Initializes all device listed for a region
 *
 *  \param region pointer to an instance of a region
 */
void acc_init_region_(struct acc_region_t_ * region);

/*!
 *  \brief Shutdown all devices of a given type
 *
 *  \param dev type of devices to initialize
 */
void acc_shutdown(acc_device_t dev);

/*!
 *  \brief Shutdown one device
 *
 *  \param dev type of the device
 *  \param num the device number
 */
void acc_shutdown_(acc_device_t dev, int num);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PUBLIC_RUNTIME */

/*! @} */

