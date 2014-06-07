
#include "OpenACC/utils/sqlite/kernel.h"

size_t kernel_entry_num_fields = 9;
char * kernel_entry_field_names[9] = {"region_id", "kernel_group_id", "kernel_id", "num_versions", "num_loops", "num_params", "num_scalars", "num_data", "name"};
enum acc_sqlite_type_e kernel_entry_field_types[9] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_text};
size_t kernel_entry_field_sizes[9] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(char[30])};
size_t kernel_entry_field_offsets[9] = {
  offsetof(struct acc_sqlite_kernel_entry_t, region_id),
  offsetof(struct acc_sqlite_kernel_entry_t, kernel_group_id),
  offsetof(struct acc_sqlite_kernel_entry_t, kernel_id),
  offsetof(struct acc_sqlite_kernel_entry_t, num_versions),
  offsetof(struct acc_sqlite_kernel_entry_t, num_loops),
  offsetof(struct acc_sqlite_kernel_entry_t, num_params),
  offsetof(struct acc_sqlite_kernel_entry_t, num_scalars),
  offsetof(struct acc_sqlite_kernel_entry_t, num_data),
  offsetof(struct acc_sqlite_kernel_entry_t, name)
};

struct acc_sqlite_kernel_entry_t * acc_sqlite_get_kernel_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id);

