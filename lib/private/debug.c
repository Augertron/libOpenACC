
#include "OpenACC/public/arch.h"
#include "OpenACC/private/debug.h"
#include "OpenACC/private/runtime.h"

#include <stdio.h>

#include <assert.h>

void acc_dbg_dump_runtime() {
  acc_init_once();

  assert(0); /// \todo useless before updating

  printf("Device type : ");
  switch (acc_runtime.curr_device_type) {
    case acc_device_any:    printf("ACC_DEVICE_ANY\n");    break;
    case acc_device_nvidia: printf("ACC_DEVICE_NVIDIA\n"); break;
    case acc_device_amd:    printf("ACC_DEVICE_AMD\n");    break;
    case acc_device_intel:  printf("ACC_DEVICE_INTEL\n");  break;
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

extern const char * cl_invalid_mem_object_tag = "CL_INVALID_MEM_OBJECT";

const char * acc_ocl_status_to_char(cl_int status) {
  char * status_str;
  switch (status) {
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return (char *)"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
      case CL_INVALID_ARG_INDEX:                         return (char *)"CL_INVALID_ARG_INDEX";
      case CL_INVALID_ARG_SIZE:                          return (char *)"CL_INVALID_ARG_SIZE";
      case CL_INVALID_ARG_VALUE:                         return (char *)"CL_INVALID_ARG_VALUE";
      case CL_INVALID_BUFFER_SIZE:                       return (char *)"CL_INVALID_BUFFER_SIZE";
      case CL_INVALID_COMMAND_QUEUE:                     return (char *)"CL_INVALID_COMMAND_QUEUE";
      case CL_INVALID_CONTEXT:                           return (char *)"CL_INVALID_CONTEXT";
      case CL_INVALID_EVENT_WAIT_LIST:                   return (char *)"CL_INVALID_EVENT_WAIT_LIST";
      case CL_INVALID_GLOBAL_OFFSET:                     return (char *)"CL_INVALID_GLOBAL_OFFSET";
      case CL_INVALID_GLOBAL_WORK_SIZE:                  return (char *)"CL_INVALID_GLOBAL_WORK_SIZE";
      case CL_INVALID_HOST_PTR:                          return (char *)"CL_INVALID_HOST_PTR";
      case CL_INVALID_IMAGE_SIZE:                        return (char *)"CL_INVALID_IMAGE_SIZE";
//    case CL_INVALID_IMAGE_FORMAT:                      return (char *)"CL_INVALID_IMAGE_FORMAT";
      case CL_INVALID_KERNEL:                            return (char *)"CL_INVALID_KERNEL";
      case CL_INVALID_KERNEL_ARGS:                       return (char *)"CL_INVALID_KERNEL_ARGS";
      case CL_INVALID_MEM_OBJECT:                        return cl_invalid_mem_object_tag;        // return (char *)"CL_INVALID_MEM_OBJECT";
      case CL_INVALID_PROGRAM_EXECUTABLE:                return (char *)"CL_INVALID_PROGRAM_EXECUTABLE";
      case CL_INVALID_SAMPLER:                           return (char *)"CL_INVALID_SAMPLER";
      case CL_INVALID_VALUE:                             return (char *)"CL_INVALID_VALUE";
      case CL_INVALID_WORK_DIMENSION:                    return (char *)"CL_INVALID_WORK_DIMENSION";
      case CL_INVALID_WORK_GROUP_SIZE:                   return (char *)"CL_INVALID_WORK_GROUP_SIZE";
      case CL_INVALID_WORK_ITEM_SIZE:                    return (char *)"CL_INVALID_WORK_ITEM_SIZE";
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:             return (char *)"CL_MEM_OBJECT_ALLOCATION_FAILURE";
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:              return (char *)"CL_MISALIGNED_SUB_BUFFER_OFFSET";
      case CL_OUT_OF_HOST_MEMORY:                        return (char *)"CL_OUT_OF_HOST_MEMORY";
      case CL_OUT_OF_RESOURCES:                          return (char *)"CL_OUT_OF_RESOURCES";
      default:                                           return (char *)"CL_UNKNOWN_ERROR_CODE";
    }
  return (char *)"CL_UNKNOWN_ERROR_CODE";
}

