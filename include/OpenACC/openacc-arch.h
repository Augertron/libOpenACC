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
 * \note Added a target language on another dimension (could also add available device runtime).
 */
typedef enum {
  acc_device_maker_unknown = 0x0001,
  acc_device_maker_nvidia  = 0x0002,
      acc_device_nvidia    = acc_device_maker_nvidia,
  acc_device_maker_radeon  = 0x0003,
      acc_device_radeon    = acc_device_maker_radeon,
  acc_device_maker_xeonphi = 0x0004,
      acc_device_xeonphi   = acc_device_maker_xeonphi,
  acc_device_maker_last    = 0x0005,

  acc_device_lang_unknown  = 0x0010,
  acc_device_lang_opencl   = 0x0020,
  acc_device_lang_cuda     = 0x0030,
  acc_device_lang_ptx      = 0x0040,
  acc_device_lang_last     = 0x0050,

  acc_device_unknown       = acc_device_maker_unknown & acc_device_lang_unknown,
  acc_device_last          = acc_device_maker_last    & acc_device_lang_last
} acc_device_t;

#endif /* __OPENACC_ARCH_H__ */ /** @} */

