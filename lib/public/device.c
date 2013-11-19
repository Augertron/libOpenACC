
#include "OpenACC/openacc.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <assert.h>

int acc_get_num_devices(acc_device_t dev) {
  acc_init_once();

  return acc_runtime.devices[dev].num;
}

void acc_set_device_type(acc_device_t dev) {
  acc_init_once();

  acc_runtime.curr_device_type = dev;
}

acc_device_t acc_get_device_type() {
  acc_init_once();

  return acc_runtime.curr_device_type;
}

void acc_set_device_num (int num, acc_device_t dev) {
  acc_init_once();

  acc_runtime.curr_device_type = dev;
  acc_runtime.curr_device_num  = num;
}

int acc_get_device_num(acc_device_t dev) {
  acc_init_once();

  assert(acc_runtime.curr_device_type == dev);

  return acc_runtime.curr_device_num;
}

int acc_on_device(acc_device_t dev) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

