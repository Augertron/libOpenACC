
#include "OpenACC/utils/profiling.h"

#include <assert.h>

char * profiling_db_file;

void init_profiling() {
  assert(profiling_db_file != NULL);

  /// \todo [profiling] init_profiling()
}

void exit_profiling() {
  assert(profiling_db_file != NULL);

  /// \todo [profiling] exit_profiling()
}

/// \todo [profiling] add profiling function for kernel and data transfert

