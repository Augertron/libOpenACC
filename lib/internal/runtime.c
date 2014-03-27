
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <assert.h>

size_t acc_get_device_idx(acc_device_t dev, int num) {
  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices); /// \todo clean error

  return first_device + num;
}

acc_device_t acc_get_device_type_by_device_idx(size_t device_idx) {
  acc_device_t dev = acc_device_any;
  
  while (dev != acc_device_last && acc_runtime.devices[dev].first < device_idx) dev++;

  assert(dev != acc_device_any && dev != acc_device_last);

  return dev-1;
}

int acc_device_idx_check_type(size_t device_idx, acc_device_t dev) {
  return device_idx >= acc_runtime.devices[dev].first && device_idx < acc_runtime.devices[dev].first + acc_runtime.devices[dev].num;
}

