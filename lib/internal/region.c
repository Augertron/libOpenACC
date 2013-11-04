
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/compiler.h"
#include "OpenACC/public/arch.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/region.h"

#include <stdio.h>
#include <string.h>

#include <assert.h>

#ifndef PRINT_BUILD_LOG
# define PRINT_BUILD_LOG 1
#endif
#if PRINT_BUILD_LOG
#include "OpenACC/internal/opencl-debug.h"
#endif

void acc_get_region_defaults(struct acc_region_t_ * region, acc_device_t dev) {
  if (region->num_gang == 0) {
    region->num_gang = acc_device_defaults[dev].num_gang;
  }
  if (region->num_worker == 0) {
    region->num_worker = acc_device_defaults[dev].num_worker;
  }
  if (region->vector_length == 0) {
    region->vector_length = acc_device_defaults[dev].vector_length;
  }
}

void acc_region_init(size_t region_id, acc_device_t dev, int num) {
  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices);

  unsigned device_idx = first_device + num;

  assert(acc_runtime.opencl_data->devices_data[device_idx] != NULL);

  if (acc_runtime.opencl_data->devices_data[device_idx]->programs[region_id] == NULL) {
    cl_context * context = &(acc_runtime.opencl_data->devices_data[device_idx]->context);
    cl_program * program = &(acc_runtime.opencl_data->devices_data[device_idx]->programs[region_id]);

    cl_int status;

    char * ocl_sources[2] = {
      acc_runtime.opencl_data->runtime_sources,
      acc_runtime.opencl_data->region_sources[region_id]
    };

    *program = clCreateProgramWithSource(
                             *context,
                             2,
                             ocl_sources,
                             NULL,
                             &status
                           );
    if (status != CL_SUCCESS) {
      printf("[fatal]   clCreateProgramWithSource : %s, %u for region %u return %u : failed\n", acc_device_name[dev], num, region_id, status);
      exit(-1);
    }

    char build_options[1024];
    build_options[0] = '\0';
    strcpy(build_options, "-I");
    strcat(build_options, compiler_data.acc_runtime_dir);
    strcat(build_options, "/include/ ");
    strcat(build_options, "-DOPENACC_RUNTIME=20131031 "); /// \todo based on MACRO

    assert(compiler_data.regions[region_id]->num_options == 0 || compiler_data.regions[region_id]->options != NULL);

    unsigned i;
    for (i = 0; i < compiler_data.regions[region_id]->num_options; i++) {
      strcat(build_options, compiler_data.regions[region_id]->options[i]);
      strcat(build_options, " ");
    }

    status = clBuildProgram(*program, 1, &(acc_runtime.opencl_data->devices[0][device_idx]), build_options, NULL, NULL);
#if PRINT_BUILD_LOG
    acc_dbg_ocl_build_log(device_idx, *program, dev, num);
#endif
    if (status != CL_SUCCESS) {
      printf("[fatal]   clBuildProgram : %s, %u for region %u return %u : failed\n", acc_device_name[dev], num, region_id, status);
      exit(-1);
    }
  }
}

