
#include "OpenACC/internal/data-env.h"
#include "OpenACC/internal/runtime.h"
#include "OpenACC/private/debug.h"

#include <stddef.h>

#include <assert.h>

void acc_push_data_environment() {

//acc_dbg_dump_data_env("before acc_push_data_environment");

  acc_init_(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  assert(data_environment != NULL);
  assert(data_environment->child == NULL);

  data_environment->child = acc_build_data_environment(data_environment);
  data_environment->child->parent = data_environment;

  data_environment = data_environment->child;

//acc_dbg_dump_data_env("after  acc_push_data_environment");
}

void acc_pop_data_environment() {

//acc_dbg_dump_data_env("before acc_pop_data_environment");

  size_t device_idx = acc_get_device_idx(acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  assert(acc_runtime.opencl_data->devices_data[device_idx]->command_queue != NULL);
  clFinish(acc_runtime.opencl_data->devices_data[device_idx]->command_queue);

  assert(data_environment != NULL);
  assert(data_environment->child == NULL);

  acc_clear_data_environment(data_environment);

  if (data_environment->parent != NULL) {
    data_environment = data_environment->parent;

    size_t num_devices = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];
    size_t i;
    for (i = 0; i < num_devices; i++)
      set_free(data_environment->child->data_allocs[i]);
    free(data_environment->child->data_allocs);
    free(data_environment->child);
    data_environment->child = NULL;
  }

//acc_dbg_dump_data_env("after  acc_pop_data_environment");
}

