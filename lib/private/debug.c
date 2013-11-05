
#include "OpenACC/public/arch.h"
#include "OpenACC/private/debug.h"
#include "OpenACC/private/runtime.h"

#include <stdio.h>

#include <assert.h>

void acc_dbg_dump_runtime() {
  printf("Device type : ");
  switch (acc_runtime.curr_device_type) {
    case acc_device_any:     printf("ACC_DEVICE_ANY\n");     break;
    case acc_device_nvidia:  printf("ACC_DEVICE_NVIDIA\n");  break;
    case acc_device_radeon:  printf("ACC_DEVICE_RADEON\n");  break;
    case acc_device_xeonphi: printf("ACC_DEVICE_XEONPHI\n"); break;
    default:
      printf("invalid !\n");
  }

  printf("Device Number : %u\n", acc_runtime.curr_device_num);
  
  if (acc_runtime.opencl_data != NULL) {
    printf("OpenCL Data:\n");
    unsigned i, j;
    for (i = 0; i < acc_runtime.opencl_data->num_platforms; i++) {
      char buf[100];
      printf("  Platform %u: \n", i);
      clGetPlatformInfo(acc_runtime.opencl_data->platforms[i], CL_PLATFORM_VENDOR, sizeof(buf), buf, NULL);
      printf("    Vendor: %s\n", buf);
      clGetPlatformInfo(acc_runtime.opencl_data->platforms[i], CL_PLATFORM_NAME, sizeof(buf), buf, NULL);
      printf("    Name: %s\n", buf);
      printf("    Number Devices: %u\n", acc_runtime.opencl_data->num_devices[i]);

      for (j = 0; j < acc_runtime.opencl_data->num_devices[i]; j++) {
        printf("    Device %u: \n", j);
        clGetDeviceInfo((acc_runtime.opencl_data->devices)[i][j], CL_DEVICE_VENDOR, sizeof(buf), buf, NULL);
        printf("      Vendor: %s\n", buf);
        clGetDeviceInfo((acc_runtime.opencl_data->devices)[i][j], CL_DEVICE_NAME, sizeof(buf), buf, NULL);
        printf("      Name: %s\n", buf);
      }
    }
  }
  else
    printf("No OpenCL Data !\n");
}

