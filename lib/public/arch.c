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

const char * acc_device_env_name [acc_device_last+1] = {
  "ANY",
  "NVIDIA",
  "C870",
  "M2070",
  "K20XM",
  "QUADRO-1000M",
  "QUADRO-4000",
  "8800GT",
  "GTX-460",
  "GTX-760",
  "AMD",
  "INTEL",
  "I5-670",
  "I7-2720QM",
  "I7-3610QM",
  "I7-950",
  "E5335",
  "E5-2620",
  "E5-2670",
  "XEONPHI",
  "ALTERA",
  ""
};

const char * acc_device_name [acc_device_last+1] = {
  "All",
    "Nvidia",
      "Tesla C870",
      "Tesla M2070",
      "Tesla K20Xm",
      "Quadro 1000M",
      "Quadro 4000",
      "GeForce 8800 GT",
      "GeForce GTX 460",
      "GeForce GTX 760",
    "AMD",
    "Intel(R)",
      "Intel(R) Core(TM) i5 CPU 670",
      "Intel(R) Core(TM) i7-2720QM CPU",
      "Intel(R) Core(TM) i7-3610QM CPU",
      "Intel(R) Core(TM) i7 CPU 950",
      "Intel(R) Xeon(R) CPU E5335",
      "Intel(R) Xeon(R) CPU E5-2620",
      "Intel(R) Xeon(R) CPU E5-2670",
      "Intel(R) XeonPhi(TM)",
    "Altera",
    "Unknown Device"
};

device_desc_t devices_desc [ACC_NUM_DEVICES] = {
  { "Tesla C870"                                      , acc_device_C870      },
  { "Tesla M2070"                                     , acc_device_M2070     },
  { "Tesla K20Xm"                                     , acc_device_K20Xm     },
  { "Quadro 1000M"                                    , acc_device_1000M     },
  { "Quadro 4000"                                     , acc_device_4000      },
  { "GeForce 8800 GT"                                 , acc_device_8800GT    },
  { "GeForce GTX 460"                                 , acc_device_GTX_460   },
  { "GeForce GTX 760"                                 , acc_device_GTX_760   },
  { "Intel(R) Core(TM) i5 CPU         670"            , acc_device_i5_670    },
  { "Intel(R) Core(TM) i7-2720QM CPU"                 , acc_device_i7_2720QM },
  { "Intel(R) Core(TM) i7-3610QM CPU"                 , acc_device_i7_3610QM },
  { "Intel(R) Core(TM) i7 CPU         950"            , acc_device_i7_950    },
  { "Intel(R) Xeon(R) CPU           E5335"            , acc_device_xeon_E5335},
  { "Intel(R) Xeon(R) CPU E5-2620"                    , acc_device_e5_2620   },
  { "Intel(R) Xeon(R) CPU E5-2670"                    , acc_device_e5_2670   },
  { "Intel(R) Many Integrated Core Acceleration Card" , acc_device_xeonphi   }
};

platform_desc_t platforms_desc[ACC_NUM_PLATFORMS] = {
  { "NVIDIA CUDA",     acc_device_nvidia, 8, &(devices_desc[0]) },
  { "AMD",             acc_device_amd,    0, NULL               }, /// \todo find actual platform name for AMD
  { "Intel(R) OpenCL", acc_device_intel,  8, &(devices_desc[8]) },
  { "Altera",          acc_device_altera, 0, NULL               }  /// \todo find actual platform name for Altera
};

acc_device_defaults_t acc_device_defaults [acc_device_last+1] = {
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
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 },
  { 1, {1, 1, 1}, {1, 1, 1}, 1 }
};

/** @} */

