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

#ifdef __cplusplus
extern "C" {
#endif

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

