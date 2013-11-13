/*!
 * \addtogroup grp_libopenacc_impl_host Host Side
 * @{
 * 
 * \file libopenacc-arch.c
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "OpenACC/public/arch.h"

const char * acc_device_env_name [acc_device_last] = {
  "ANY",
  "NVIDIA",
  "AMD",
  "RADEON",
  "INTEL",
  "CORE",
  "I5-670",
  "I7-3610QM",
  "I7-950",
  "XEONPHI"
};

const char * acc_device_name [acc_device_last] = {
  "All",
  "Nvidia",
  "AMD",
  "AMD radeon",
  "Intel(R)",
  "Intel(R) Core(TM)",
  "Intel(R) Core(TM) i5 CPU 670",
  "Intel(R) Core(TM) i7-3610QM CPU",
  "Intel(R) Core(TM) i7 CPU 950",
  "Intel(R) XeonPhi(TM)"
};

acc_device_defaults_t acc_device_defaults [acc_device_last] = {
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
  { 1, {0, 0, 0}, {0, 0, 0}, 0 },
};

device_desc_t devices_desc [2] = {
  { "Intel(R) Core(TM) i5 CPU         670" , acc_device_i5_670    },
  { "Intel(R) Core(TM) i7-3610QM CPU"      , acc_device_i7_3610QM },
  { "Intel(R) Core(TM) i7 CPU         950" , acc_device_i7_950    }
};

device_type_desc_t devices_type_desc [12] = {
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL               }, ///< \todo NVidia GPU
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_GPU         , acc_device_radeon  , 0, NULL               }, ///< Radeon
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_CPU         , acc_device_core    , 3, &(devices_desc[0]) }, ///< Core
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_xeonphi , 0, NULL               }, ///< XeonPhi
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL               }
};

platform_desc_t platforms_desc[NUM_OPENCL_PLATFORMS] = {
  { "Nvidia",          acc_device_nvidia, 3, &(devices_type_desc[0]) }, /// \todo find actual platform name for Nvidia
  { "AMD",             acc_device_amd,    3, &(devices_type_desc[3]) }, /// \todo find actual platform name for AMD
  { "Intel(R) OpenCL", acc_device_intel,  3, &(devices_type_desc[6]) },
  { "Altera",          acc_device_altera, 3, &(devices_type_desc[9]) }  /// \todo find actual platform name for Altera
};

/** @} */

