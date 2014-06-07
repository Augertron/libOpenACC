
#ifndef __LIBOPENACC_SQLITE_KERNEL_H__
#define __LIBOPENACC_SQLITE_KERNEL_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_kernel_entry_t {
  size_t region_id;
  size_t kernel_group_id;
  size_t kernel_id;
  size_t num_versions;
  size_t num_loops;
  size_t num_params;
  size_t num_scalars;
  size_t num_datas;
  char name[30];
};
extern size_t kernel_entry_num_fields;
extern char * kernel_entry_field_names[9];
extern enum acc_sqlite_type_e kernel_entry_field_types[9];
extern size_t kernel_entry_field_sizes[9];
extern size_t kernel_entry_field_offsets[9];

struct acc_sqlite_kernel_entry_t * acc_sqlite_get_kernel_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_KERNEL_H__ */

