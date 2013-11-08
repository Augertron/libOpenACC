
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

char * acc_ocl_status_to_char(cl_int status) {
  char * status_str;
  switch (status) {
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: status_str = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"; break;
      case CL_INVALID_ARG_INDEX:                         status_str = "CL_INVALID_ARG_INDEX";                         break;
      case CL_INVALID_ARG_SIZE:                          status_str = "CL_INVALID_ARG_SIZE";                          break;
      case CL_INVALID_ARG_VALUE:                         status_str = "CL_INVALID_ARG_VALUE";                         break;
      case CL_INVALID_BUFFER_SIZE:                       status_str = "CL_INVALID_BUFFER_SIZE";                       break;
      case CL_INVALID_COMMAND_QUEUE:                     status_str = "CL_INVALID_COMMAND_QUEUE";                     break;
      case CL_INVALID_CONTEXT:                           status_str = "CL_INVALID_CONTEXT";                           break;
      case CL_INVALID_EVENT_WAIT_LIST:                   status_str = "CL_INVALID_EVENT_WAIT_LIST";                   break;
      case CL_INVALID_GLOBAL_OFFSET:                     status_str = "CL_INVALID_GLOBAL_OFFSET";                     break;
      case CL_INVALID_GLOBAL_WORK_SIZE:                  status_str = "CL_INVALID_GLOBAL_WORK_SIZE";                  break;
      case CL_INVALID_HOST_PTR:                          status_str = "CL_INVALID_HOST_PTR";                          break;
      case CL_INVALID_IMAGE_SIZE:                        status_str = "CL_INVALID_IMAGE_SIZE";                        break;
//    case CL_INVALID_IMAGE_FORMAT:                      status_str = "CL_INVALID_IMAGE_FORMAT";                      break;
      case CL_INVALID_KERNEL:                            status_str = "CL_INVALID_KERNEL";                            break;
      case CL_INVALID_KERNEL_ARGS:                       status_str = "CL_INVALID_KERNEL_ARGS";                       break;
      case CL_INVALID_MEM_OBJECT:                        status_str = "CL_INVALID_MEM_OBJECT";                        break;
      case CL_INVALID_PROGRAM_EXECUTABLE:                status_str = "CL_INVALID_PROGRAM_EXECUTABLE";                break;
      case CL_INVALID_SAMPLER:                           status_str = "CL_INVALID_SAMPLER";                           break;
      case CL_INVALID_VALUE:                             status_str = "CL_INVALID_VALUE";                             break;
      case CL_INVALID_WORK_DIMENSION:                    status_str = "CL_INVALID_WORK_DIMENSION";                    break;
      case CL_INVALID_WORK_GROUP_SIZE:                   status_str = "CL_INVALID_WORK_GROUP_SIZE";                   break;
      case CL_INVALID_WORK_ITEM_SIZE:                    status_str = "CL_INVALID_WORK_ITEM_SIZE";                    break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:             status_str = "CL_MEM_OBJECT_ALLOCATION_FAILURE";             break;
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:              status_str = "CL_MISALIGNED_SUB_BUFFER_OFFSET";              break;
      case CL_OUT_OF_HOST_MEMORY:                        status_str = "CL_OUT_OF_HOST_MEMORY";                        break;
      case CL_OUT_OF_RESOURCES:                          status_str = "CL_OUT_OF_RESOURCES";                          break;
      default:                                           status_str = "CL_UNKNOWN_ERROR_CODE";                        break;
    }
  return status_str;
}

