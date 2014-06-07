
#ifndef __LIBOPENACC_SQLITE_LOOP_H__
#define __LIBOPENACC_SQLITE_LOOP_H__

#include "OpenACC/utils/sqlite/sqlite.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_loop_entry_t {
  size_t region_id;
  size_t kernel_group_id;
  size_t kernel_id;
  size_t version_id;
  size_t loop_id;
};
extern size_t loop_entry_num_fields;
extern char * loop_entry_field_names[5];
extern enum acc_sqlite_type_e loop_entry_field_types[5];
extern size_t loop_entry_field_sizes[5];
extern size_t loop_entry_field_offsets[5];

struct acc_sqlite_loop_entry_t * acc_sqlite_get_loop_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id, size_t version_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_LOOP_H__ */

