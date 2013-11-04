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
#include "OpenACC/private/init.h"
#include "OpenACC/private/debug.h"

#include <stdio.h>

int main() {
  acc_init_once();

  printf("\n\n*********************\n\nDump runtime data:\n");

  acc_dbg_dump_runtime();

  int num;
  int success = 0;

  printf("\n*********************\n\nCheck present devices:\n\n");

  num = acc_get_num_devices(acc_device_any);
  printf("  acc_get_num_devices(acc_device_any)       = %u\n", num);

  num = acc_get_num_devices(acc_device_nvidia);
  printf("  acc_get_num_devices(acc_device_nvidia)    = %u\n", num);

  num = acc_get_num_devices(acc_device_amd);
  printf("  acc_get_num_devices(acc_device_amd)       = %u\n", num);

  num = acc_get_num_devices(acc_device_radeon);
  printf("  acc_get_num_devices(acc_device_radeon)    = %u\n", num);

  num = acc_get_num_devices(acc_device_intel);
  printf("  acc_get_num_devices(acc_device_intel)     = %u\n", num);

  num = acc_get_num_devices(acc_device_core);
  printf("  acc_get_num_devices(acc_device_core)      = %u\n", num);

  num = acc_get_num_devices(acc_device_i7_3610QM);
  printf("  acc_get_num_devices(acc_device_i7_3610QM) = %u\n", num);

  num = acc_get_num_devices(acc_device_i7_950);
  printf("  acc_get_num_devices(acc_device_i7_950)    = %u\n", num);

  num = acc_get_num_devices(acc_device_xeonphi);
  printf("  acc_get_num_devices(acc_device_xeonphi)   = %u\n", num);

  num = acc_get_num_devices(acc_device_altera);
  printf("  acc_get_num_devices(acc_device_altera)    = %u\n", num);

  printf("\n%u successful API call out of 9\n\n*********************\n", success);

  acc_init(acc_device_any); // Initializes every devices

  return 0;
}

/*! @} */

