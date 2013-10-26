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

#include <CL/cl.h>

/* Represent a type of accelerator (the API says "a value to tell [..] what type of device" API 2.0 at 3.2.3)
 * \note Added a target language on another "dimension". It is just an idea (not used for now)
 */
typedef enum acc_device_e {
  acc_device_any = 0,           ///< All devices
  acc_device_nvidia,            ///< Nvidia devices \todo list type/model
  acc_device_amd,               ///< AMD devices \todo more type ?
    acc_device_radeon,          ///<   Radeon \todo list model
  acc_device_intel,             ///< Intel devices
    acc_device_core,            ///<   Intel Core Processor \todo list models
      acc_device_i7_3610QM,     ///<     Intel(R) Core(TM) i7-3610QM CPU
      acc_device_i7_950,        ///<     Intel(R) Core(TM) i7 950 CPU
    acc_device_xeonphi,         ///<   Intel Xeon Processor \todo list models
  acc_device_altera,            ///< Altera devices \todo list type/model
  acc_device_last
} acc_device_t;

extern const char * acc_device_env_name [acc_device_last];

extern const char * acc_device_name [acc_device_last];

struct device_desc_t_ {
  const char * ocl_name; ///< Name of a specific model of CPU/GPU/Accelerator (can be partial)

  acc_device_t device;
};
typedef const struct device_desc_t_ device_desc_t;

struct device_type_desc_t_ {
  cl_device_type ocl_device_type; ///< The type of device in OpenCL classification

  acc_device_t device;

  unsigned num_devices;
  const device_desc_t * devices_desc;
};
typedef const struct device_type_desc_t_ device_type_desc_t;

struct platform_desc_t_ {
  const char * ocl_name;

  acc_device_t device;

  unsigned num_devices_type;
  const device_type_desc_t * devices_type_desc;
};
typedef const struct platform_desc_t_ platform_desc_t;

#define NUM_OPENCL_PLATFORMS 4

extern platform_desc_t platforms_desc[NUM_OPENCL_PLATFORMS];

extern device_type_desc_t devices_type_desc[12];

extern device_desc_t devices_desc[2];

#endif /* __OPENACC_ARCH_H__ */ /** @} */

