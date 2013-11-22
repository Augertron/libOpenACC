
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/compiler.h"
#include "OpenACC/public/arch.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/region.h"
#include "OpenACC/internal/opencl-debug.h"
#include "OpenACC/private/debug.h"

#include <stdio.h>
#include <string.h>

#include <assert.h>

#ifndef PRINT_BUILD_LOG
# define PRINT_BUILD_LOG 0
#endif

void acc_get_region_defaults(struct acc_region_t_ * region) {
  if (region->num_dims == 0 || region->num_dims == acc_device_defaults[acc_runtime.curr_device_type].num_dims) {
    region->num_dims = acc_device_defaults[acc_runtime.curr_device_type].num_dims;
    size_t i;
    for (i = 0; i < region->num_dims; i++) {
      if (region->num_gang[i] == 0)
        region->num_gang[i] = acc_device_defaults[acc_runtime.curr_device_type].num_gang[i];
      if (region->num_worker[i] == 0)
        region->num_worker[i] = acc_device_defaults[acc_runtime.curr_device_type].num_worker[i];
    }
  }
  if (region->vector_length == 0)
    region->vector_length = acc_device_defaults[acc_runtime.curr_device_type].vector_length;
}

void acc_region_init(struct acc_region_t_ * region) {
  assert(acc_runtime.opencl_data->devices_data[region->device_idx] != NULL);

  if (acc_runtime.opencl_data->devices_data[region->device_idx]->programs[region->desc->id] == NULL) {
    cl_context * context = &(acc_runtime.opencl_data->devices_data[region->device_idx]->context);
    cl_program * program = &(acc_runtime.opencl_data->devices_data[region->device_idx]->programs[region->desc->id]);

    cl_int status;

    char * ocl_sources[2] = {
      acc_runtime.opencl_data->runtime_sources,
      acc_runtime.opencl_data->region_sources[region->desc->id]
    };

    *program = clCreateProgramWithSource(*context, 2, (const char **)ocl_sources, NULL, &status);
    if (status != CL_SUCCESS) {
      printf("[fatal]   clCreateProgramWithSource : %s, %u for region %u return %u : failed\n",
             acc_device_name[acc_runtime.curr_device_type], acc_runtime.curr_device_num, region->desc->id, status);
      exit(-1);
    }

    char build_options[1024];
    build_options[0] = '\0';
    strcpy(build_options, "-I");
    strcat(build_options, compiler_data.acc_runtime_dir);
    strcat(build_options, "/include/ ");

    assert(compiler_data.regions[region->desc->id]->num_options == 0 || compiler_data.regions[region->desc->id]->options != NULL);

    unsigned i;
    for (i = 0; i < compiler_data.regions[region->desc->id]->num_options; i++) {
      strcat(build_options, compiler_data.regions[region->desc->id]->options[i]);
      strcat(build_options, " ");
    }

    status = clBuildProgram(*program, 1, &(acc_runtime.opencl_data->devices[0][region->device_idx]), build_options, NULL, NULL);
#if PRINT_BUILD_LOG
    acc_dbg_ocl_build_log(region->device_idx, *program, acc_runtime.curr_device_type, acc_runtime.curr_device_num);
#endif
    if (status != CL_SUCCESS) {
      printf("[fatal]   clBuildProgram : %s, %u for region %u return %u : failed\n",
             acc_device_name[acc_runtime.curr_device_type], acc_runtime.curr_device_num, region->desc->id, status);
      exit(-1);
    }
  }
}

