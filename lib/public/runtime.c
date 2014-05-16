
#include "OpenACC/openacc.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/region.h"
#include "OpenACC/private/debug.h"

#include "OpenACC/internal/compiler.h"
#include "OpenACC/utils/profiling.h"

#include <stdio.h>

#include <assert.h>

#ifndef PRINT_INFO
# define PRINT_INFO 0
#endif

#ifndef PRINT_BUILD_LOG
# define PRINT_BUILD_LOG 0
#endif

acc_runtime_t acc_runtime = {0, NULL, acc_device_any, 0}; // by default, we use the first of any device

void acc_init(acc_device_t dev) {
  acc_init_once();

  size_t num_devices = acc_get_num_devices(dev);

  assert(num_devices > 0);

  unsigned i;
  for (i = 0; i < num_devices; i++)
    acc_init_(dev, i);
}

static void acc_ocl_ctx_error_callback(
  const char * errinfo, 
  const void * private_info, 
  size_t cb, 
  void * user_data
) {
  printf("[error]   acc_ocl_ctx_error_callback : %s\n", errinfo);
}

void acc_init__(unsigned device_idx) { 
  acc_init_once();

  if (acc_runtime.opencl_data->devices_data[device_idx] == NULL) {
    cl_int status;

    cl_device_id * device = &(acc_runtime.opencl_data->devices[0][device_idx]);

    acc_device_data_t device_data = (acc_device_data_t)malloc(sizeof(struct acc_device_data_t_));
    if (device_data == NULL) {
      perror("[fatal]   malloc : device_data");
      exit(-1);
    }
    acc_runtime.opencl_data->devices_data[device_idx] = device_data;

    device_data->context = clCreateContext(NULL, 1, device, &acc_ocl_ctx_error_callback, NULL, &status);
    if (status != CL_SUCCESS || device_data->context == NULL) {
       printf("[error]   clCreateContext : %s, %u return %u : failed\n", "", 0/** \todo acc_device_name[dev], num*/, status);

       free(device_data);
       device_data = NULL;

       return;
    }

    device_data->programs = (cl_program *)malloc(compiler_data.num_regions * sizeof(cl_program));
    if (device_data->programs == NULL) {
      perror("[fatal]   malloc : device_data->programs");
      exit(-1);
    }
    unsigned i;
    for (i = 0; i < compiler_data.num_regions; i++)
      device_data->programs[i] = NULL;

    device_data->command_queue = clCreateCommandQueue(device_data->context, *device, CL_QUEUE_PROFILING_ENABLE, &status);
    if (status != CL_SUCCESS || device_data->command_queue == NULL) {
      char * status_str;
      switch (status) {
        case CL_INVALID_CONTEXT:          status_str = "CL_INVALID_CONTEXT";          break;
        case CL_INVALID_DEVICE:           status_str = "CL_INVALID_DEVICE";           break;
        case CL_INVALID_VALUE:            status_str = "CL_INVALID_VALUE";            break;
        case CL_INVALID_QUEUE_PROPERTIES: status_str = "CL_INVALID_QUEUE_PROPERTIES"; break;
        case CL_OUT_OF_RESOURCES:         status_str = "CL_OUT_OF_RESOURCES";         break;
        case CL_OUT_OF_HOST_MEMORY:       status_str = "CL_OUT_OF_HOST_MEMORY";       break;
        default:                          status_str = "CL_UNKNOWN_ERROR_CODE";       break;
      }
      printf("[fatal  ] clCreateCommandQueue... return %s\n", status_str);
      exit(-1); /// \todo error code
    }
  }
}

void acc_init_(acc_device_t dev, int num) {
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(dev, num);

  acc_init__(device_idx);
}

void acc_init_region_(struct acc_region_t_ * region) {
  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx++)
    acc_init__(region->devices[idx].device_idx);
}

void acc_shutdown(acc_device_t dev) {
  acc_init_once();

  size_t num_devices = acc_get_num_devices(dev);

  assert(num_devices > 0);
  unsigned i;
  for (i = 0; i < num_devices; i++)
    acc_shutdown_(dev, i);
}

void acc_shutdown_(acc_device_t dev, int num) {
#if PRINT_INFO
  printf("[warning] Shuting OpenACC down...\n");
#endif
  acc_init_once();

  unsigned device_idx = acc_get_device_idx(dev, num);

  if (acc_runtime.opencl_data->devices_data[device_idx] != NULL) {
    cl_int status;

    status = clReleaseContext(acc_runtime.opencl_data->devices_data[device_idx]->context);
    if (status != CL_SUCCESS)
      printf("[warning] clReleaseContext : %s, %u return %u\n", acc_device_name[dev], num, status);
    if (acc_runtime.opencl_data->devices_data[device_idx]->programs != NULL) {
      unsigned i;
      for (i = 0; i < compiler_data.num_regions; i++)
        if (acc_runtime.opencl_data->devices_data[device_idx]->programs[i] != NULL) {
          status = clReleaseProgram(acc_runtime.opencl_data->devices_data[device_idx]->programs[i]);
          if (status != CL_SUCCESS)
            printf("[warning] clReleaseProgram : %s, %u for region %u return %u\n", acc_device_name[dev], num, i, status);
        }
      free(acc_runtime.opencl_data->devices_data[device_idx]->programs);
      acc_runtime.opencl_data->devices_data[device_idx]->programs = NULL;
    }
    free(acc_runtime.opencl_data->devices_data[device_idx]);

    acc_runtime.opencl_data->devices_data[device_idx] = NULL;
  }
}

