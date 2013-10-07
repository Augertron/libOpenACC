/*!
 * \file openacc-arch.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 * 
 * Standardized OpenACC C API
 *
 */

#ifndef __OPENACC_ARCH_H__
#define __OPENACC_ARCH_H__

/// Represent a type of accelerator (the API says "a value to tell [..] what type of device" API 2.0 at 3.2.3)
typedef enum {
  acc_device_unknown = 0,
  acc_device_nvidia,
  acc_device_radeon,
  acc_device_xeonphi,
  acc_device_last
} acc_device_t;

#endif /* __OPENACC_ARCH_H__ */

