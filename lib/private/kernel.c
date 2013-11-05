
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
  assert(acc_runtime.opencl_data->devices_data[region->device_idx] != NULL);
  assert(acc_runtime.opencl_data->devices_data[region->device_idx]->command_queue != NULL);

  // Create a default context
  acc_context_t context = acc_create_context(region, kernel);

  // Look for a matching ‭version of the kernel, fill the context according to the selected version
  cl_kernel ocl_kernel = acc_build_ocl_kernel(region, kernel, context);

  /// \todo set kernel arguments

  size_t global_work_size[1] = { region->num_gang[0] * region->num_worker[0] };
  size_t local_work_size[1] = { region->num_worker[0] };

  cl_int status = clEnqueueNDRangeKernel(
    acc_runtime.opencl_data->devices_data[region->device_idx]->command_queue,
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
    char * status_str;
    switch (status) {
      case CL_INVALID_PROGRAM_EXECUTABLE:    status_str = "CL_INVALID_PROGRAM_EXECUTABLE";    break;
      case CL_INVALID_COMMAND_QUEUE:         status_str = "CL_INVALID_COMMAND_QUEUE";         break;
      case CL_INVALID_KERNEL:                status_str = "CL_INVALID_KERNEL";                break;
      case CL_INVALID_CONTEXT:               status_str = "CL_INVALID_CONTEXT";               break;
      case CL_INVALID_KERNEL_ARGS:           status_str = "CL_INVALID_KERNEL_ARGS";           break;
      case CL_INVALID_WORK_DIMENSION:        status_str = "CL_INVALID_WORK_DIMENSION";        break;
      case CL_INVALID_GLOBAL_WORK_SIZE:      status_str = "CL_INVALID_GLOBAL_WORK_SIZE";      break;
      case CL_INVALID_GLOBAL_OFFSET:         status_str = "CL_INVALID_GLOBAL_OFFSET";         break;
      case CL_INVALID_WORK_GROUP_SIZE:       status_str = "CL_INVALID_WORK_GROUP_SIZE";       break;
      case CL_INVALID_WORK_ITEM_SIZE:        status_str = "CL_INVALID_WORK_ITEM_SIZE";        break;
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:  status_str = "CL_MISALIGNED_SUB_BUFFER_OFFSET";  break;
      case CL_INVALID_IMAGE_SIZE:            status_str = "CL_INVALID_IMAGE_SIZE";            break;
//    case CL_INVALID_IMAGE_FORMAT:          status_str = "CL_INVALID_IMAGE_FORMAT";          break;
      case CL_OUT_OF_RESOURCES:              status_str = "CL_OUT_OF_RESOURCES";              break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE: status_str = "CL_MEM_OBJECT_ALLOCATION_FAILURE"; break;
      case CL_INVALID_EVENT_WAIT_LIST:       status_str = "CL_INVALID_EVENT_WAIT_LIST";       break;
      case CL_OUT_OF_HOST_MEMORY:            status_str = "CL_OUT_OF_HOST_MEMORY";            break;
      default:                               status_str = "CL_UNKNOWN_ERROR_CODE";            break;
    }
    printf("[fatal]   clEnqueueNDRangeKernel return %s for region[%u].kernel[%u].\n", status_str, region->desc->id, kernel->desc->id);
    exit(-1); /// \todo error code
  }

  assert(!"NIY"); /// \todo
}

