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
  "M2070",
  "K20XM",
  "QUADRO-1000M",
  "QUADRO-4000",
  "GTX-460",
  "AMD",
  "INTEL",
  "I5-670",
  "I7-2720QM",
  "I7-3610QM",
  "I7-950",
  "E5-2620",
  "E5-2670",
  "XEONPHI"
};

const char * acc_device_name [acc_device_last] = {
  "All",
    "Nvidia",
      "Tesla M2070",
      "Tesla K20Xm",
      "Quadro 1000M",
      "Quadro 4000",
      "GeForce GTX 460",
    "AMD",
    "Intel(R)",
      "Intel(R) Core(TM) i5 CPU 670",
      "Intel(R) Core(TM) i7-2720QM CPU",
      "Intel(R) Core(TM) i7-3610QM CPU",
      "Intel(R) Core(TM) i7 CPU 950",
      "Intel(R) Xeon(R) CPU E5-2620",
      "Intel(R) Xeon(R) CPU E5-2670",
      "Intel(R) XeonPhi(TM)"
};

device_desc_t devices_desc [12] = {
  { "Tesla M2070"                                     , acc_device_M2070     },
  { "Tesla K20Xm"                                     , acc_device_K20Xm     },
  { "Quadro 1000M"                                    , acc_device_1000M     },
  { "Quadro 4000"                                     , acc_device_4000      },
  { "GeForce GTX 460"                                 , acc_device_GTX_460   },
  { "Intel(R) Core(TM) i5 CPU         670"            , acc_device_i5_670    },
  { "Intel(R) Core(TM) i7-2720QM CPU"                 , acc_device_i7_2720QM },
  { "Intel(R) Core(TM) i7-3610QM CPU"                 , acc_device_i7_3610QM },
  { "Intel(R) Core(TM) i7 CPU         950"            , acc_device_i7_950    },
  { "Intel(R) Xeon(R) CPU E5-2620"                    , acc_device_e5_2620   },
  { "Intel(R) Xeon(R) CPU E5-2670"                    , acc_device_e5_2670   },
  { "Intel(R) Many Integrated Core Acceleration Card" , acc_device_xeonphi   }
};

platform_desc_t platforms_desc[NUM_OPENCL_PLATFORMS] = {
  { "NVIDIA CUDA",     acc_device_nvidia, 5, &(devices_desc[0]) },
  { "AMD",             acc_device_amd,    0, NULL               }, /// \todo find actual platform name for AMD
  { "Intel(R) OpenCL", acc_device_intel,  7, &(devices_desc[5]) },
  { "Altera",          acc_device_altera, 0, NULL               }  /// \todo find actual platform name for Altera
};

acc_device_defaults_t acc_device_defaults [acc_device_last] = {
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 }
};

/** @} */

