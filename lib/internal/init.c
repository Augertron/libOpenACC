
#include "OpenACC/internal/init.h"

#include "OpenACC/public/arch.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/internal/opencl-init.h"
#include "OpenACC/private/debug.h"

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
  /// \todo Set runtime defaults from environment variables

  acc_runtime.check_list |= f_acc_defaults;
}

