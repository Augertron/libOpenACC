
#ifndef __LIBOPENACC_SQLITE_TILE_H__
#define __LIBOPENACC_SQLITE_TILE_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_tile_entry_t {
  size_t region_id;
  size_t kernel_group_id;
  size_t kernel_id;
  size_t version_id;
  size_t loop_id;
  size_t tile_id;
  size_t position;
  size_t kind;
  size_t param;
};
extern size_t tile_entry_num_fields;
extern char * tile_entry_field_names[9];
extern enum acc_sqlite_type_e tile_entry_field_types[9];
extern size_t tile_entry_field_sizes[9];
extern size_t tile_entry_field_offsets[9];

struct acc_sqlite_tile_entry_t * acc_sqlite_get_tile_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id, size_t version_id, size_t loop_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_TILE_H__ */

