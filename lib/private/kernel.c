
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/device/host-context.h"

#include "OpenACC/private/runtime.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_region_t_ * acc_region_t;
typedef struct acc_context_t_ * acc_context_t;

acc_kernel_t acc_build_kernel(acc_kernel_desc_t kernel) {
  acc_init_once();

  acc_kernel_t result = (acc_kernel_t)malloc(sizeof(struct acc_kernel_t_));

  result->desc = kernel;

  result->scalar_ptrs = (  void **)malloc(kernel->num_scalars * sizeof(  void *));
  result->data_ptrs   = (d_void **)malloc(kernel->num_datas   * sizeof(d_void *));

  result->loops = (acc_loop_desc_t *)malloc(kernel->num_loops * sizeof(struct acc_loop_desc_t_ *));
  unsigned i;
  for (i = 0; i < kernel->num_loops; i++)
    result->loops[i] = (acc_loop_desc_t)malloc(sizeof(struct acc_loop_desc_t_));

  return result;
}

void acc_enqueue_kernel(acc_region_t region, acc_kernel_t kernel) {
  acc_init_once();

  // Checks
  assert(region->num_dims == 1);
  assert(region->num_gang[0] > 0 && region->num_worker[0] > 0 && region->vector_length > 0);
  assert(kernel->loops[0]->stride == 1); /// \todo currently only support loops with positive unit stride

  // Create a default context
  acc_context_t context = acc_create_context(region, kernel);

  // Look for a matching ‭version of the kernel, fill the context according to the selected version
  cl_kernel ocl_kernel = acc_build_ocl_kernel(region, kernel, context);

  cl_int status = clEnqueueNDRangeKernel(
    acc_runtime.opencl_data->command_queue,
    ocl_kernel,
    /* cl_uint work_dim = */ 1,
    /* const size_t * global_work_offset = */ NULL,
    /* const size_t * global_work_size = */ NULL,
    /* const size_t * local_work_size = */ NULL,
    /* cl_uint num_events_in_wait_list = */ 0,
    /* const cl_event * event_wait_list = */ NULL,
    /* cl_event * event = */ NULL
  );
  if (status != CL_SUCCESS) {
    printf("[fatal]   clEnqueueNDRangeKernel return %u for region[%u].kernel[%u].\n", status, region->desc->id, kernel->desc->id);
    exit(-1); /// \todo error code
  }

  assert(!"NIY"); /// \todo
}

