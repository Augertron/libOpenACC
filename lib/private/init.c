
#include "OpenACC/private/init.h"
#include "OpenACC/internal/init.h"
#include "OpenACC/private/debug.h"

#include "OpenACC/internal/data-env.h"

#include "OpenACC/public/arch.h"
#include "OpenACC/private/runtime.h"

#include "OpenACC/utils/profiling.h"
#include "OpenACC/utils/sqlite/sqlite.h"

#include <assert.h>

void acc_init_once() {
  if (!check_flag(f_alloc)) {
    acc_runtime.opencl_data = (acc_opencl_data_t)malloc(sizeof(struct acc_opencl_data_t_));
    set_flag(f_alloc);
  }

  if (!check_flag(f_acc_sqlite)) {
    acc_sqlite_init();
    set_flag(f_acc_sqlite);
  }

  if (!check_flag(f_ocl_devices))
    acc_collect_ocl_devices();
  assert(check_flag(f_ocl_devices));

  if (!check_flag(f_acc_devices))
    acc_init_devices();
  assert(check_flag(f_acc_devices));

  if (!check_flag(f_ocl_sources))
    acc_load_ocl_sources();
  assert(check_flag(f_ocl_sources));

  if (!check_flag(f_acc_defaults))
    acc_init_defaults();
  assert(check_flag(f_acc_defaults));

  if (!check_flag(f_mem_manager))
    init_memory_manager();
  assert(check_flag(f_mem_manager));

  if (!check_flag(f_data_env)) {
    data_environment = acc_build_data_environment(NULL);
    assert(data_environment != NULL);
    set_flag(f_data_env);
  }

  if (!check_flag(f_acc_profiling)){
    acc_profiling_init();
    set_flag(f_acc_profiling);
  }
}

