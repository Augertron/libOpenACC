
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <assert.h>

acc_device_t acc_get_device_type_by_device_idx(size_t device_idx) {
  acc_device_t dev = acc_device_any + 1;

  do {
    if (device_idx - acc_runtime.devices[dev].first < acc_runtime.devices[dev].num) {
      dev++; // found platform : first device type in platform
      while (dev != acc_device_last) {
        if (device_idx - acc_runtime.devices[dev].first < acc_runtime.devices[dev].num)
          break;
        dev++;
      }
      break;
    }
    else dev++;
  } while (dev != acc_device_last);

  return dev;
}

int acc_device_idx_check_type(size_t device_idx, acc_device_t dev) {
  return device_idx >= acc_runtime.devices[dev].first && device_idx < acc_runtime.devices[dev].first + acc_runtime.devices[dev].num;
}

