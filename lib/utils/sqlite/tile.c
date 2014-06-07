
#include "OpenACC/utils/sqlite/tile.h"

size_t tile_entry_num_fields = 9;
char * tile_entry_field_names[9] = {
  "region_id", "kernel_group_id", "kernel_id", "version_id", "loop_id", "tile_id", "position", "kind", "param"
};
enum acc_sqlite_type_e tile_entry_field_types[9] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t tile_entry_field_sizes[9] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t tile_entry_field_offsets[9] = {
  offsetof(struct acc_sqlite_tile_entry_t, region_id),
  offsetof(struct acc_sqlite_tile_entry_t, kernel_group_id),
  offsetof(struct acc_sqlite_tile_entry_t, kernel_id),
  offsetof(struct acc_sqlite_tile_entry_t, version_id),
  offsetof(struct acc_sqlite_tile_entry_t, loop_id),
  offsetof(struct acc_sqlite_tile_entry_t, tile_id),
  offsetof(struct acc_sqlite_tile_entry_t, position),
  offsetof(struct acc_sqlite_tile_entry_t, kind),
  offsetof(struct acc_sqlite_tile_entry_t, param)
};

struct acc_sqlite_tile_entry_t * acc_sqlite_get_tile_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id, size_t version_id, size_t loop_id);

