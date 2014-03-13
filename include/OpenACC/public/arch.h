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

#ifndef OPENACC_PUBLIC_ARCH
#define OPENACC_PUBLIC_ARCH 20131031

#include <CL/cl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Represent a type of accelerator (the API says "a value to tell [..] what type of device" API 2.0 at 3.2.3)
 * \note Added a target language on another "dimension". It is just an idea (not used for now)
 */
typedef enum acc_device_e {
  acc_device_any = 0,           ///< All devices
  acc_device_nvidia,
      acc_device_M2070,
      acc_device_K20Xm,
      acc_device_1000M,
      acc_device_GTX_460,
  acc_device_amd,               ///< AMD devices \todo more type ?
  acc_device_intel,             ///< Intel devices
      acc_device_i5_670,        ///      Intel(R) Core(TM) i5 670 CPU \bug with valgrind this is returned instead of my i73610QM !
      acc_device_i7_2720QM,
      acc_device_i7_3610QM,     ///<     Intel(R) Core(TM) i7-3610QM CPU
      acc_device_i7_950,        ///<     Intel(R) Core(TM) i7 950 CPU
      acc_device_e5_2620,
      acc_device_e5_2670,
      acc_device_xeonphi,       ///<   Intel Xeon Processor \todo list models
  acc_device_altera,            ///< Altera devices \todo list type/model
  acc_device_last
} acc_device_t;

/// List of environment names for the different supported devices
extern const char * acc_device_env_name [acc_device_last];

/// Listing name for the different devices (for printing)
extern const char * acc_device_name [acc_device_last];

/// Default number of Gang, Worker, and Vector
struct acc_device_defaults_t_ {
  unsigned num_dims;
  unsigned long num_gang[3];
  unsigned long num_worker[3];
  unsigned long vector_length;
};
typedef const struct acc_device_defaults_t_ acc_device_defaults_t;

/// List of default number of Gang, Worker, and Vector for each supported device.
extern acc_device_defaults_t acc_device_defaults [acc_device_last];

/// Descriptor for a specific device 
struct device_desc_t_ {
  const char * ocl_name; ///< Name of a specific model of CPU/GPU/Accelerator (can be partial)

  acc_device_t device;
};
typedef const struct device_desc_t_ device_desc_t;

/// Descriptor of a vendor platform
struct platform_desc_t_ {
  const char * ocl_name;

  acc_device_t device;

  unsigned num_devices;
  const device_desc_t * devices_desc;
};
typedef const struct platform_desc_t_ platform_desc_t;

#define NUM_OPENCL_PLATFORMS 4

/// List of supported platforms
extern platform_desc_t platforms_desc[NUM_OPENCL_PLATFORMS];

/// List of supported devices
extern device_desc_t devices_desc[11];

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PUBLIC_ARCH */

/** @} */

