
#include "OpenACC/utils/sqlite/loop.h"

size_t loop_entry_num_fields = 5;
char * loop_entry_field_names[5] = {
  "region_id", "kernel_group_id", "kernel_id", "version_id", "loop_id"
};
enum acc_sqlite_type_e loop_entry_field_types[5] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t loop_entry_field_sizes[5] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t loop_entry_field_offsets[5] = {
  offsetof(struct acc_sqlite_loop_entry_t, region_id),
  offsetof(struct acc_sqlite_loop_entry_t, kernel_group_id),
  offsetof(struct acc_sqlite_loop_entry_t, kernel_id),
  offsetof(struct acc_sqlite_loop_entry_t, version_id),
  offsetof(struct acc_sqlite_loop_entry_t, loop_id)
};

struct acc_sqlite_loop_entry_t * acc_sqlite_get_loop_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id, size_t version_id);

