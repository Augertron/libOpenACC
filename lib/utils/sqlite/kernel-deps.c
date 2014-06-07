
#include "OpenACC/utils/sqlite/kernel-deps.h"

size_t kernel_deps_entry_num_fields = 4;
char * kernel_deps_entry_field_names[4] = {"region_id", "kernel_group_id", "kernel_id", "predecessor_kernel_id"};
enum acc_sqlite_type_e kernel_deps_entry_field_types[4] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t kernel_deps_entry_field_sizes[4] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t kernel_deps_entry_field_offsets[4] = {
  offsetof(struct acc_sqlite_kernel_deps_entry_t, region_id),
  offsetof(struct acc_sqlite_kernel_deps_entry_t, kernel_group_id),
  offsetof(struct acc_sqlite_kernel_deps_entry_t, kernel_id),
  offsetof(struct acc_sqlite_kernel_deps_entry_t, predecessor_kernel_id)
};

struct acc_sqlite_kernel_deps_entry_t * acc_sqlite_get_kernel_deps_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id);

