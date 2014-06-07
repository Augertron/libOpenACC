
#include "OpenACC/utils/sqlite/kernel-data.h"

size_t kernel_data_entry_num_fields = 5;
char * kernel_data_entry_field_names[5] = {"region_id", "kernel_group_id", "kernel_id", "data_id", "idx_region"};
enum acc_sqlite_type_e kernel_data_entry_field_types[5] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t kernel_data_entry_field_sizes[5] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t kernel_data_entry_field_offsets[5] = {
  offsetof(struct acc_sqlite_kernel_data_entry_t, region_id),
  offsetof(struct acc_sqlite_kernel_data_entry_t, kernel_group_id),
  offsetof(struct acc_sqlite_kernel_data_entry_t, kernel_id),
  offsetof(struct acc_sqlite_kernel_data_entry_t, data_id),
  offsetof(struct acc_sqlite_kernel_data_entry_t, idx_region)
};

struct acc_sqlite_kernel_data_entry_t * acc_sqlite_get_kernel_data_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id);

