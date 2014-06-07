
#include "OpenACC/utils/sqlite/kernel-group.h"

size_t kernel_group_entry_num_fields = 2;
char * kernel_group_entry_field_names[2] = { "run_id", "device_id" };
enum acc_sqlite_type_e kernel_group_entry_field_types[2] = {e_sqlite_int, e_sqlite_int};
size_t kernel_group_entry_field_sizes[2] = {sizeof(size_t), sizeof(size_t)};
size_t kernel_group_entry_field_offsets[2] = {
  offsetof(struct acc_sqlite_kernel_group_entry_t, region_id),
  offsetof(struct acc_sqlite_kernel_group_entry_t, kernel_group_id)
};

struct acc_sqlite_kernel_group_entry_t * acc_sqlite_get_kernel_group_entries(sqlite3 * db, size_t * count, size_t region_id);

