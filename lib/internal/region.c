
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
# define PRINT_BUILD_LOG 1
#endif

void acc_get_region_defaults(struct acc_region_t_ * region) {
  unsigned i;
  for (i = 0; i < region->num_devices; i++) {
    if (region->devices[i].num_gang == 0)
      region->devices[i].num_gang = acc_device_defaults[region->desc->devices[i].kind].num_gang;
    assert(region->devices[i].num_gang > 0);
    if (region->devices[i].num_worker == 0)
      region->devices[i].num_worker = acc_device_defaults[region->desc->devices[i].kind].num_worker;
    assert(region->devices[i].num_worker > 0);
    if (region->devices[i].vector_length == 0)
      region->devices[i].vector_length = acc_device_defaults[region->desc->devices[i].kind].vector_length;
    assert(region->devices[i].vector_length > 0);
  }
}

void acc_region_init(struct acc_region_t_ * region) {

  char * ocl_sources[2] = {
    acc_runtime.opencl_data->runtime_sources,
    acc_runtime.opencl_data->region_sources[region->desc->id]
  };

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

  cl_int status;

  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx ++) {
    size_t device_idx = region->devices[idx].device_idx;

    assert(acc_runtime.opencl_data->devices_data[device_idx] != NULL);

    if (acc_runtime.opencl_data->devices_data[device_idx]->programs[region->desc->id] == NULL) {
      cl_context * context = &(acc_runtime.opencl_data->devices_data[device_idx]->context);
      cl_program * program = &(acc_runtime.opencl_data->devices_data[device_idx]->programs[region->desc->id]);

      *program = clCreateProgramWithSource(*context, 2, (const char **)ocl_sources, NULL, &status);
      if (status != CL_SUCCESS) {
        printf("[fatal]   clCreateProgramWithSource : %s, %u for region %u return %u : failed\n",
               acc_device_name[acc_runtime.curr_device_type], acc_runtime.curr_device_num, region->desc->id, status);
        exit(-1);
      }

      status = clBuildProgram(*program, 1, &(acc_runtime.opencl_data->devices[0][device_idx]), build_options, NULL, NULL);
#if PRINT_BUILD_LOG
      /// \todo cannot rely on acc_runtime.curr_device_type and acc_runtime.curr_device_num
      acc_dbg_ocl_build_log(device_idx, *program, acc_runtime.curr_device_type, acc_runtime.curr_device_num);
#endif
      if (status != CL_SUCCESS) {
        printf("[fatal]   clBuildProgram : %s, %u for region %u return %u : failed\n",
               acc_device_name[acc_runtime.curr_device_type], acc_runtime.curr_device_num, region->desc->id, status);
        exit(-1);
      }
    }
  }
}

