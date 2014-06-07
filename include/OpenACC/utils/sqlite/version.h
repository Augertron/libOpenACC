
#ifndef __LIBOPENACC_SQLITE_VERSION_H__
#define __LIBOPENACC_SQLITE_VERSION_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_version_entry_t {
  size_t region_id;
  size_t kernel_group_id;
  size_t kernel_id;
  size_t version_id;
  size_t num_gang[3];
  size_t num_worker[3];
  size_t vector_length;
  char suffix[30];
};
extern size_t version_entry_num_fields;
extern char * version_entry_field_names[12];
extern enum acc_sqlite_type_e version_entry_field_types[12];
extern size_t version_entry_field_sizes[12];
extern size_t version_entry_field_offsets[12];

struct acc_sqlite_version_entry_t * acc_sqlite_get_version_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_VERSION_H__ */

