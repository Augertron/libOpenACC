/*!
 * \addtogroup grp_libopenacc_api_public_arch
 * @{
 * 
 * \file openacc-arch.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 *
 */

#ifndef __OPENACC_ARCH_H__
#define __OPENACC_ARCH_H__

/* Represent a type of accelerator (the API says "a value to tell [..] what type of device" API 2.0 at 3.2.3)
 * \note Added a target language on another "dimension". It is just an idea (not used for now)
 */
typedef enum acc_device_e {
  acc_device_any     = 0x0000,
  acc_device_nvidia  = 0x0001,
  acc_device_radeon  = 0x0002,
  acc_device_xeonphi = 0x0003,
  acc_device_last
} acc_device_t;

#endif /* __OPENACC_ARCH_H__ */ /** @} */

