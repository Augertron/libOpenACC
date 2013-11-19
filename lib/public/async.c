
#include "OpenACC/openacc.h"
#include "OpenACC/private/debug.h"

#include <assert.h>

int acc_async_test(int id) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

int acc_async_test_all() {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

void acc_async_wait(int id) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_async_wait_async(int id_wait, int id_async) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_async_wait_all() {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_async_wait_all_async(int id) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

