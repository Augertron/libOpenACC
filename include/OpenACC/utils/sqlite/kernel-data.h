
#ifndef __LIBOPENACC_SQLITE_KERNEL_DATA_H__
#define __LIBOPENACC_SQLITE_KERNEL_DATA_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_kernel_data_entry_t {
  size_t region_id;
  size_t kernel_group_id;
  size_t kernel_id;
  size_t data_id;
  size_t idx_region;
};
extern size_t kernel_data_entry_num_fields;
extern char * kernel_data_entry_field_names[5];
extern enum acc_sqlite_type_e kernel_data_entry_field_types[5];
extern size_t kernel_data_entry_field_sizes[5];
extern size_t kernel_data_entry_field_offsets[5];

struct acc_sqlite_kernel_data_entry_t * acc_sqlite_get_kernel_data_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_KERNEL_DATA_H__ */

