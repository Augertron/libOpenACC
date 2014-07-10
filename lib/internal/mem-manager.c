
#include "OpenACC/internal/runtime.h"
#include "OpenACC/internal/mem-manager.h"
#include "OpenACC/utils/containers/bimap.h"
#include "OpenACC/internal/init.h"
#include "OpenACC/utils/utils.h"
#include "OpenACC/private/debug.h"

#include <assert.h>

// **********************

typedef struct acc_data_allocation_t_ * acc_data_allocation_t;

// **********************

/// Array of bimap (one per device) : host_ptr -> data_alloc AND dev_ptr -> data_alloc
bimap_t * data_ptrs_bimap = NULL;

void init_memory_manager() {
  size_t nbr_devices = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];
  size_t i;

  data_ptrs_bimap = (bimap_t *)malloc(nbr_devices * sizeof(bimap_t));
  for (i = 0; i < nbr_devices; i++)
    data_ptrs_bimap[i] = bimap_alloc(42, sizeof(h_void *), sizeof(d_void *),
                                         sizeof(struct acc_data_allocation_t_),
                                         (key_cmp_f)&void_ptr_cmp, (key_cmp_f)&void_ptr_cmp);

  set_flag(f_mem_manager);
}

void free_memory_manager() {
  size_t nbr_devices = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];
  size_t i;
  for (i = 0; i < nbr_devices; i++)
    bimap_free(data_ptrs_bimap[i]);
  free(data_ptrs_bimap);
}

// **********************

void register_data_allocation(
  unsigned device_idx,
   const struct acc_data_allocation_t_ * data_alloc
) {
#if DBG_DATA
  printf("[debug] register_data_allocation(device_idx = %u, data_alloc->host_ptr = %x, data_alloc->dev_ptr = %x, data_alloc->size = %u)\n", device_idx, data_alloc->host_ptr, data_alloc->dev_ptr, data_alloc->size);
#endif
  bimap_insert(data_ptrs_bimap[device_idx], &(data_alloc->host_ptr), &(data_alloc->dev_ptr), data_alloc);
}

void remove_data_allocation(unsigned device_idx, const h_void * host_ptr) {
#if DBG_DATA
  printf("[debug] remove_data_allocation(device_idx = %u, host_ptr = %x)\n", device_idx, host_ptr);
#endif

  bimap_remove_by_key_1(data_ptrs_bimap[device_idx], &host_ptr);
}

d_void * acc_get_deviceptr(unsigned device_idx, const h_void * host_ptr) {
  acc_data_allocation_t data_alloc = bimap_get_value_by_key_1(data_ptrs_bimap[device_idx], &host_ptr);
  return data_alloc != NULL ? data_alloc->dev_ptr : NULL;
}

h_void * acc_get_hostptr(unsigned device_idx, const d_void * device_ptr) {
  acc_data_allocation_t data_alloc = bimap_get_value_by_key_2(data_ptrs_bimap[device_idx], &device_ptr);
  return data_alloc != NULL ? data_alloc->host_ptr : NULL;
}

int acc_check_present(unsigned device_idx, const h_void * host_ptr, size_t n) {
  acc_data_allocation_t data_alloc = bimap_get_value_by_key_1(data_ptrs_bimap[device_idx], &host_ptr);
  return data_alloc != NULL ? data_alloc->size == n : 0;
}

