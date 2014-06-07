
#include "OpenACC/utils/sqlite/region-parameter.h"

size_t region_parameter_entry_num_fields = 3;
char * region_parameter_entry_field_names[3] = {"region_id", "param_id", "size"};
enum acc_sqlite_type_e region_parameter_entry_field_types[3] = {e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t region_parameter_entry_field_sizes[3] = {sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t region_parameter_entry_field_offsets[3] = {
  offsetof(struct acc_sqlite_region_parameter_entry_t, region_id),
  offsetof(struct acc_sqlite_region_parameter_entry_t, param_id),
  offsetof(struct acc_sqlite_region_parameter_entry_t, size)
};

struct acc_sqlite_region_parameter_entry_t * acc_sqlite_get_region_parameter_entries(sqlite3 * db, size_t * count, size_t region_id);

