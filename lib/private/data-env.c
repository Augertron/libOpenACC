
#include "OpenACC/internal/data-env.h"
#include "OpenACC/internal/allocator.h"
#include "OpenACC/internal/runtime.h"

#include <stddef.h>

#include <assert.h>

void acc_push_data_environment() {
  if (data_environment == NULL) {
    data_environment = acc_build_data_environment(NULL);
  }
  else {
    assert(data_environment->child == NULL);
    data_environment->child = acc_build_data_environment(data_environment);
  }
}

void acc_pop_data_environment() {
  assert(data_environment != NULL);
  assert(data_environment->child == NULL);

  /// \todo clean data_environment

  if (data_environment->parent != NULL) {
    data_environment = data_environment->parent;
    data_environment->child = NULL;
  }
}

