/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   OpenACC/public/device.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_PUBLIC_DEVICE
#define OPENACC_PUBLIC_DEVICE 20131031

#include "OpenACC/public/def.h"
#include "OpenACC/public/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Get the number of present devices of a given type.
 *
 *  OpenACC API 2.0 at 3.2.1
 *
 *  \param dev  an OpenACC device
 *  \return number of present devices of the same type
 */
int acc_get_num_devices(acc_device_t dev);

/*!
 *  \brief Set the device to be used in the following parallel regions
 *
 *  \note OpenACC API 2.0 at 3.2.2
 *
 *  \param dev  an OpenACC device
 */
void acc_set_device_type(acc_device_t dev);

/*!
 *  \brief Get the current type of device
 *
 *  \note OpenACC API 2.0 at 3.2.3
 *
 *  \return the device to be used in the following parallel regions
 */
acc_device_t acc_get_device_type();

/*!
 *  \brief Tell the runtime which device of a certain type should be used in the next accelerator section. 
 *
 *  \note OpenACC API 2.0 at 3.2.4
 *
 *  \note Implies a call to acc_set_device_type()
 *
 *  \param id   device number of the device to use
 *  \param dev  an OpenACC device type
 */
void acc_set_device_num(int id, acc_device_t dev);

/*!
 *  \brief Get the device number that will be used in the next section
 *
 *  \note OpenACC API 2.0 at 3.2.5
 *
 *  \note What is the behavior if dev is different from acc_get_device_type() ?
 *
 *  \param dev an OpenACC device type
 *  \return which device will be used in the next accelerator section
 */
int acc_get_device_num (acc_device_t dev);

/*!
 */
int acc_on_device(acc_device_t);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PUBLIC_DEVICE */

/*! @} */

