
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

#ifndef DBG_REGION
# define DBG_REGION 0
#endif

#ifndef BUILD_LOG
# define BUILD_LOG 0
#endif

struct acc_region_desc_t_ * acc_region_desc_by_ID(size_t region_id) {
  size_t i;
  for (i = 0; i < compiler_data.num_regions; i++)
    if (compiler_data.regions[i]->id == region_id)
      return compiler_data.regions[i];
  return NULL;
}

void acc_get_region_defaults(struct acc_region_t_ * region) {
  unsigned i;
  for (i = 0; i < region->num_devices; i++) {
    if (region->devices[i].num_gang[0] == 0)
      region->devices[i].num_gang[0] = acc_device_defaults[region->desc->devices[i].kind].num_gang[0];
    if (region->devices[i].num_gang[1] == 0)
      region->devices[i].num_gang[1] = acc_device_defaults[region->desc->devices[i].kind].num_gang[1];
    if (region->devices[i].num_gang[2] == 0)
      region->devices[i].num_gang[2] = acc_device_defaults[region->desc->devices[i].kind].num_gang[2];

    if (region->devices[i].num_worker[0] == 0)
      region->devices[i].num_worker[0] = acc_device_defaults[region->desc->devices[i].kind].num_worker[0];
    if (region->devices[i].num_worker[1] == 0)
      region->devices[i].num_worker[1] = acc_device_defaults[region->desc->devices[i].kind].num_worker[1];
    if (region->devices[i].num_worker[2] == 0)
      region->devices[i].num_worker[2] = acc_device_defaults[region->desc->devices[i].kind].num_worker[2];

    if (region->devices[i].vector_length == 0)
      region->devices[i].vector_length = acc_device_defaults[region->desc->devices[i].kind].vector_length;
  }
}

void acc_region_init(struct acc_region_t_ * region) {
#if DBG_REGION
  printf("[debug]  acc_region_init\n");
#endif

  size_t region_id;
  for (region_id = 0; region_id < compiler_data.num_regions; region_id++)
    if (compiler_data.regions[region_id]->id == region->desc->id)
      break;
  assert(region_id < compiler_data.num_regions);

  char * ocl_sources[2] = {
    acc_runtime.opencl_data->runtime_sources,
    acc_runtime.opencl_data->region_sources[region_id]
  };

  char build_options[1024];
  build_options[0] = '\0';
  strcpy(build_options, "-I");
  strcat(build_options, compiler_data.acc_runtime_dir);
  strcat(build_options, "/include/ ");

  assert(compiler_data.regions[region_id]->num_options == 0 || compiler_data.regions[region_id]->options != NULL);

  unsigned i;
  for (i = 0; i < compiler_data.regions[region_id]->num_options; i++) {
    assert(strlen(build_options) < 512);
    strcat(build_options, compiler_data.regions[region_id]->options[i]);
    strcat(build_options, " ");
  }

  cl_int status;

  unsigned idx;
  for (idx = 0; idx < region->num_devices; idx ++) {
    size_t device_idx = region->devices[idx].device_idx;

    assert(acc_runtime.opencl_data->devices_data[device_idx] != NULL);

    if (acc_runtime.opencl_data->devices_data[device_idx]->programs[region_id] == NULL) {
      cl_context * context = &(acc_runtime.opencl_data->devices_data[device_idx]->context);
      cl_program * program = &(acc_runtime.opencl_data->devices_data[device_idx]->programs[region_id]);

      *program = clCreateProgramWithSource(*context, 2, (const char **)ocl_sources, NULL, &status);
      if (status != CL_SUCCESS) {
        const char * status_str = acc_ocl_status_to_char(status);
        printf("[fatal]   clCreateProgramWithSource on %s (#%u) for region %zd return %s : failed\n",
               acc_device_name[acc_runtime.curr_device_type], acc_runtime.curr_device_num, region_id, status_str);
        exit(-1);
      }

      status = clBuildProgram(*program, 1, &(acc_runtime.opencl_data->devices[0][device_idx]), build_options, NULL, NULL);
      if (status == CL_BUILD_PROGRAM_FAILURE)
        acc_dbg_ocl_build_log(device_idx, *program);
      if (status != CL_SUCCESS) {
        const char * status_str = acc_ocl_status_to_char(status);
        printf("[fatal]   clBuildProgram on %s (#%u) for region %zd return %s\n",
               acc_device_name[acc_runtime.curr_device_type], acc_runtime.curr_device_num, region_id, status_str);
        exit(-1);
      }

      cl_build_status build_status;
      status = clGetProgramBuildInfo(*program, acc_runtime.opencl_data->devices[0][device_idx], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
      if (status != CL_SUCCESS) {
        const char * status_str = acc_ocl_status_to_char(status);
        printf("[fatal]   clGetProgramBuildInfo return %s\n", status_str);
        exit(-1); /// \todo error code
      }
      switch (build_status) {
        case CL_BUILD_NONE:
          printf("[fatal]   clGetProgramBuildInfo: build status: CL_BUILD_NONE.\n");
          exit(-1);
        case CL_BUILD_ERROR:
          printf("[fatal]   clGetProgramBuildInfo: build status: CL_BUILD_ERROR.\n");
          acc_dbg_ocl_build_log(device_idx, *program);
          exit(-1);
        case CL_BUILD_SUCCESS:
          printf("[info]    clGetProgramBuildInfo: build status: CL_BUILD_SUCCESS.\n");
#if BUILD_LOG
          acc_dbg_ocl_build_log(device_idx, *program);
#endif
          break;
        case CL_BUILD_IN_PROGRESS: // Should not append as we do not provide a callback function to clBuildProgram
          printf("[fatal]   clGetProgramBuildInfo: build status: CL_BUILD_IN_PROGRESS.\n"\
                 "[fatal]                          It should not append as we do not provide a callback function to clBuildProgram");
        default:
          assert(0);
      }
    }
  }
}

