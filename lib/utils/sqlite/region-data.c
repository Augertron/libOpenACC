
#include "OpenACC/utils/sqlite/region-data.h"

size_t region_data_entry_num_fields = 2;
char * region_data_entry_field_names[2] = {"region_id", "data_id"};
enum acc_sqlite_type_e region_data_entry_field_types[2] = {e_sqlite_int, e_sqlite_int};
size_t region_data_entry_field_sizes[2] = {sizeof(size_t), sizeof(size_t)};
size_t region_data_entry_field_offsets[2] = {
  offsetof(struct acc_sqlite_region_data_entry_t, region_id),
  offsetof(struct acc_sqlite_region_data_entry_t, data_id)
};

struct acc_sqlite_region_data_entry_t * acc_sqlite_get_region_data_entries(sqlite3 * db, size_t * count, size_t region_id);

