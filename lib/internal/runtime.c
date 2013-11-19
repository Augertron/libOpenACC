
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <assert.h>

size_t acc_get_device_idx(acc_device_t dev, int num) {
  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices);

  return first_device + num;
}

