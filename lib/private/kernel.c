
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/device/host-context.h"
#include "OpenACC/private/debug.h"

#include "OpenACC/private/runtime.h"

#include <stdlib.h>
#include <stdio.h>
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
  unsigned dev_idx;
  for (dev_idx = 0; dev_idx < region->num_devices; dev_idx++) {
    assert(region->devices[dev_idx].num_gang > 0);
    assert(region->devices[dev_idx].num_worker > 0);
    assert(region->devices[dev_idx].vector_length > 0);

    size_t device_idx = region->devices[dev_idx].device_idx;
    assert(acc_runtime.opencl_data->devices_data[device_idx] != NULL);

    // Create a default context
    acc_context_t context = acc_create_context(region, kernel, device_idx);

    // Look for a matching ‭version of the kernel, fill the context according to the selected version
    cl_kernel ocl_kernel = acc_build_ocl_kernel(region, kernel, context, device_idx);

    cl_int status;
    cl_uint idx = 0;
    unsigned i;

    // Set scalar kernel arguments 
    for (i = 0; i < kernel->desc->num_scalars; i++) {
      status = clSetKernelArg(ocl_kernel, idx, kernel->desc->size_scalars[i], kernel->scalar_ptrs[i]);
      if (status != CL_SUCCESS) {
        const char * status_str = acc_ocl_status_to_char(status);
        printf("[fatal]   clSetKernelArg return %s for region[%u].kernel[%u] argument %u (scalar #%u).\n",
                  status_str, region->desc->id, kernel->desc->id, idx, i
              );
        exit(-1); /// \todo error code
      }
      idx++;
    }

    // Set data kernel argument
    assert(kernel->data_ptrs[i] != NULL);
    for (i = 0; i < kernel->desc->num_datas; i++) {
      status = clSetKernelArg(ocl_kernel, idx, sizeof(cl_mem), &(kernel->data_ptrs[i]));
      if (status != CL_SUCCESS) {
        const char * status_str = acc_ocl_status_to_char(status);
        printf("[fatal]   clSetKernelArg return %s for region[%u].kernel[%u] argument %u (data #%u).\n",
                  status_str, region->desc->id, kernel->desc->id, idx, i
              );
        exit(-1); /// \todo error code
      }
      idx++;
    }

    // Allocate/copy context in constant memory \todo alloc only copy before launch with event wait
    cl_mem ocl_context = clCreateBuffer( acc_runtime.opencl_data->devices_data[device_idx]->context,
                                         CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                         sizeof(struct acc_context_t_) + context->num_loop * sizeof(struct acc_kernel_loop_t_),
                                         context, &status );
    if (status != CL_SUCCESS) {
      const char * status_str = acc_ocl_status_to_char(status);
      printf("[fatal]   clCreateBuffer return %s for region[%u].kernel[%u] when call to build the kernel copy of context.\n",
                status_str, region->desc->id, kernel->desc->id
            );
      exit(-1); /// \todo error code
    }

    // Set context of the kernel
    status = clSetKernelArg(ocl_kernel, idx, sizeof(cl_mem), &ocl_context);
    if (status != CL_SUCCESS) {
      const char * status_str = acc_ocl_status_to_char(status);
      printf("[fatal]   clSetKernelArg return %s for region[%u].kernel[%u] argument %u (context).\n",
                status_str, region->desc->id, kernel->desc->id, idx, i
            );
      exit(-1); /// \todo error code
    }
    idx++;

    assert(acc_runtime.opencl_data->devices_data[device_idx]->command_queue != NULL);

    // Launch the kernel
    size_t global_work_size[1] = { region->devices[dev_idx].num_gang * region->devices[dev_idx].num_worker };
    size_t local_work_size[1] = { region->devices[dev_idx].num_worker };
    status = clEnqueueNDRangeKernel(
      acc_runtime.opencl_data->devices_data[device_idx]->command_queue,
      ocl_kernel,
      /* cl_uint work_dim                  = */ 1,
      /* const size_t * global_work_offset = */ NULL,
      /* const size_t * global_work_size   = */ global_work_size,
      /* const size_t * local_work_size    = */ local_work_size,
      /* cl_uint num_events_in_wait_list   = */ 0,
      /* const cl_event * event_wait_list  = */ NULL,
      /* cl_event * event                  = */ NULL
    );
    if (status != CL_SUCCESS) {
      const char * status_str = acc_ocl_status_to_char(status);
      printf("[fatal]   clEnqueueNDRangeKernel return %s for region[%u].kernel[%u].\n",
                status_str, region->desc->id, kernel->desc->id
            );
      exit(-1); /// \todo error code
    }
  }
}

