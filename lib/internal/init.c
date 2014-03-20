
#include "OpenACC/internal/init.h"

#include "OpenACC/public/arch.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/internal/opencl-init.h"
#include "OpenACC/private/debug.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

int check_flag(uint32_t flag) {
  return (acc_runtime.check_list & flag) == flag;
}

void set_flag(uint32_t flag) {
  acc_runtime.check_list |= flag;
}

void acc_init_devices() {
  acc_init_ocl_devices();

  set_flag(f_acc_devices);
}

void acc_init_defaults() {
  /// \todo use secure_getenv but linking problem: only has __secure_getenv
  char * env_dev_type = getenv("ACC_DEVICE_TYPE");
  if (env_dev_type != NULL && env_dev_type[0] != "/0") {
    size_t dev_idx;
    for (dev_idx = 0; dev_idx < acc_device_last; dev_idx++)
      if (strcmp(acc_device_env_name[dev_idx], env_dev_type) == 0)
        break;
    if (dev_idx < acc_device_last)
      acc_runtime.curr_device_type = dev_idx;
  }

  char * env_dev_num = getenv("ACC_DEVICE_NUM");
  if (env_dev_num != NULL && env_dev_num[0] != "/0") {
    int dev_num = atoi(env_dev_num);
    /// \fixme atoi is not robust: what happen if env_dev_num is not a number
    acc_runtime.curr_device_num = dev_num;
  }

  set_flag(f_acc_defaults);
}

