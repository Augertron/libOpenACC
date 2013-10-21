/*!
 * \addtogroup grp_libopenacc_tests
 * @{
 *
 * \file tests/acc_device.c
 * 
 * In this file, we tests:
 *  - acc_get_num_devices : Get the number of present devices of a given type.
 *  - acc_set_device_type : Set the device to be used in the following parallel regions.
 *  - acc_get_device_type : Get the current type of device.
 *  - acc_set_device_num  : Associate an ID (integer) to a device.
 *  - acc_get_device_num  : Get ID (integer) to a device. 
 *
 */

#include "OpenACC/openacc.h"

#define NUM_DEVICES                    1
#define    NUM_NVIDIA_DEVICES          0
#define    NUM_AMD_DEVICES             0
#define       NUM_RADEON_DEVICES       0
#define    NUM_INTEL_DEVICES           1
#define       NUM_CORE_DEVICES         1
#define          NUM_I7_3610QM_DEVICES 1
#define       NUM_XEONPHI_DEVICES      0
#define    NUM_ALTERA_DEVICES          0

int main() {
  acc_init_once();

  printf("\n\n*********************\n\nDump runtime data:\n");

  acc_dbg_dump_runtime();

  int num;
  int success = 0;

  printf("\n*********************\n\nCheck present devices:\n\n");

  num = acc_get_num_devices(acc_device_any);
  printf("  acc_get_num_devices(acc_device_any)       = %u (exp. %u).\n", num, NUM_DEVICES);
  success += (num == NUM_DEVICES);

  num = acc_get_num_devices(acc_device_nvidia);
  printf("  acc_get_num_devices(acc_device_nvidia)    = %u (exp. %u).\n", num, NUM_NVIDIA_DEVICES);
  success += (num == NUM_NVIDIA_DEVICES);

  num = acc_get_num_devices(acc_device_amd);
  printf("  acc_get_num_devices(acc_device_amd)       = %u (exp. %u).\n", num, NUM_AMD_DEVICES);
  success += (num == NUM_AMD_DEVICES);

  num = acc_get_num_devices(acc_device_radeon);
  printf("  acc_get_num_devices(acc_device_radeon)    = %u (exp. %u).\n", num, NUM_RADEON_DEVICES);
  success += (num == NUM_RADEON_DEVICES);

  num = acc_get_num_devices(acc_device_intel);
  printf("  acc_get_num_devices(acc_device_intel)     = %u (exp. %u).\n", num, NUM_INTEL_DEVICES);
  success += (num == NUM_INTEL_DEVICES);

  num = acc_get_num_devices(acc_device_core);
  printf("  acc_get_num_devices(acc_device_core)      = %u (exp. %u).\n", num, NUM_CORE_DEVICES);
  success += (num == NUM_CORE_DEVICES);

  num = acc_get_num_devices(acc_device_i7_3610QM);
  printf("  acc_get_num_devices(acc_device_i7_3610QM) = %u (exp. %u).\n", num, NUM_I7_3610QM_DEVICES);
  success += (num == NUM_CORE_DEVICES);

  num = acc_get_num_devices(acc_device_xeonphi);
  printf("  acc_get_num_devices(acc_device_xeonphi)   = %u (exp. %u).\n", num, NUM_XEONPHI_DEVICES);
  success += (num == NUM_XEONPHI_DEVICES);

  num = acc_get_num_devices(acc_device_altera);
  printf("  acc_get_num_devices(acc_device_altera)    = %u (exp. %u).\n", num, NUM_ALTERA_DEVICES);
  success += (num == NUM_ALTERA_DEVICES);

  printf("\n%u successful API call out of 9\n\n*********************\n", success);

  if (success != 9) return 1;

  acc_init(acc_device_any); // Initializes every devices

  return 0;
}

/*! @} */

