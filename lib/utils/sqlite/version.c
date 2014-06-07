
#include "OpenACC/utils/sqlite/version.h"

size_t version_entry_num_fields = 12;
char * version_entry_field_names[12] = {"region_id", "kernel_group_id", "kernel_id", "version_id", "num_gang_0", "num_gang_1", "num_gang_2", "num_worker_0", "num_worker_1", "num_worker_2", "vector_length", "suffix"};
enum acc_sqlite_type_e version_entry_field_types[12] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_text};
size_t version_entry_field_sizes[12] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(char[30])};
size_t version_entry_field_offsets[12] = {
  offsetof(struct acc_sqlite_version_entry_t, region_id),
  offsetof(struct acc_sqlite_version_entry_t, kernel_group_id),
  offsetof(struct acc_sqlite_version_entry_t, kernel_id),
  offsetof(struct acc_sqlite_version_entry_t, version_id),
  offsetof(struct acc_sqlite_version_entry_t, num_gang),
  offsetof(struct acc_sqlite_version_entry_t, num_gang) + sizeof(size_t),
  offsetof(struct acc_sqlite_version_entry_t, num_gang) + 2 * sizeof(size_t),
  offsetof(struct acc_sqlite_version_entry_t, num_worker),
  offsetof(struct acc_sqlite_version_entry_t, num_worker) + sizeof(size_t),
  offsetof(struct acc_sqlite_version_entry_t, num_worker) + 2 * sizeof(size_t),
  offsetof(struct acc_sqlite_version_entry_t, vector_length),
  offsetof(struct acc_sqlite_version_entry_t, suffix)
};

struct acc_sqlite_version_entry_t * acc_sqlite_get_version_entries(sqlite3 * db, size_t * count, size_t region_id, size_t kernel_group_id, size_t kernel_id);

