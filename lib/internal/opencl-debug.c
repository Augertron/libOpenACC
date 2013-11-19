
#include "OpenACC/internal/opencl-debug.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

void acc_dbg_ocl_build_log(unsigned device_idx, cl_program program, acc_device_t dev, int num) {
  char * build_log;
  size_t build_log_size;
  clGetProgramBuildInfo(program, acc_runtime.opencl_data->devices[0][device_idx], CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
  if (build_log_size == 0)
    printf("[warning] OpenCL return an empty log...\n");
  else {
    build_log = (char*)malloc(build_log_size);
    if (build_log == NULL) {
      perror("[fatal] malloc : build_log");
      exit(-1);
    }
    clGetProgramBuildInfo(program, acc_runtime.opencl_data->devices[0][device_idx], CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    build_log[build_log_size-1] = '\0';
    printf("Device %s %u Build Log:\n%s\n", acc_device_name[dev], num, build_log);
    free(build_log);
  }
}

