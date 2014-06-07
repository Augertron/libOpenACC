
#ifndef __LIBOPENACC_SQLITE_KERNEL_DEPS_H__
#define __LIBOPENACC_SQLITE_KERNEL_DEPS_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_kernel_deps_t {
  size_t region_id;
  size_t kernel_group_id;
  size_t kernel_id;
  size_t predecessor_kernel_id;
};
extern size_t kernel_deps_entry_num_fields;
extern char * kernel_deps_entry_field_names[4];
extern enum acc_sqlite_type_e kernel_deps_entry_field_types[4];
extern size_t kernel_deps_entry_field_sizes[4];
extern size_t kernel_deps_entry_field_offsets[4];

struct acc_sqlite_kernel_deps_entry_t * acc_sqlite_get_kernel_deps_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_KERNEL_DEPS_H__ */

