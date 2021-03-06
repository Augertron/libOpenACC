
#include "OpenACC/internal/data-env.h"
#include "OpenACC/internal/mem-manager.h"
#include "OpenACC/internal/runtime.h"
#include "OpenACC/private/debug.h"

#include "OpenACC/utils/containers/set.h"
#include "OpenACC/utils/utils.h"

#include <assert.h>

struct acc_data_environment_t_ * data_environment = NULL;

struct acc_data_environment_t_ * acc_build_data_environment(struct acc_data_environment_t_ * parent) {
  struct acc_data_environment_t_ * result = (struct acc_data_environment_t_ *)malloc(sizeof(struct acc_data_environment_t_));

  result->parent = parent;
  result->child = NULL;

  size_t num_devices = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];
  result->data_allocs = (set_t *)malloc(num_devices * sizeof(set_t));
  size_t i;
  for (i = 0; i < num_devices; i++)
    result->data_allocs[i] = set_alloc(42, sizeof(void *), (key_cmp_f)&void_ptr_cmp);

  return result;
}

void acc_clear_data_environment(struct acc_data_environment_t_ * data_env) {

//acc_dbg_dump_data_env("before acc_clear_data_environment");

  size_t num_devices = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];
  size_t i, j;
  for (i = 0; i < num_devices; i++) {
    for (j = 0; j < data_env->data_allocs[i]->count; j++) {
      h_void * host_ptr = *(void **)(data_env->data_allocs[i]->datas + j * data_env->data_allocs[i]->storage_size);
      d_void * dev_ptr = acc_get_deviceptr(i, host_ptr);
      acc_free_(i, dev_ptr);
      remove_data_allocation(i, host_ptr);
    }
    data_env->data_allocs[i]->count = 0;
  }

//acc_dbg_dump_data_env("after  acc_clear_data_environment");
}

void acc_map_data_(unsigned device_idx, h_void * host_ptr, d_void * dev_ptr, size_t n) {
#if DBG_DATA
  printf("[debug] acc_map_data_(device_idx = %u, host_ptr = %x, dev_ptr = %x, n = %u)\n", device_idx, host_ptr, dev_ptr, n);
#endif

  struct acc_data_allocation_t_ alloc = { host_ptr, dev_ptr, n };

  register_data_allocation(device_idx, &alloc);

  set_insert(data_environment->data_allocs[device_idx], &host_ptr);
}

void acc_unmap_data_(unsigned device_idx, h_void * host_ptr) {
  remove_data_allocation(device_idx, host_ptr);

  set_remove(data_environment->data_allocs[device_idx], &host_ptr);
}

