
#include "OpenACC/utils/sqlite/region.h"

size_t region_entry_num_fields = 6;
char * region_entry_field_names[6] = {"region_id", "num_loops", "num_params", "num_scalars", "num_datas", "opencl_file"};
enum acc_sqlite_type_e region_entry_field_types[6] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_text};
size_t region_entry_field_sizes[6] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(char[50])};
size_t region_entry_field_offsets[6] = {
  offsetof(struct acc_sqlite_region_entry_t, region_id),
  offsetof(struct acc_sqlite_region_entry_t, num_kernels),
  offsetof(struct acc_sqlite_region_entry_t, num_params),
  offsetof(struct acc_sqlite_region_entry_t, num_scalars),
  offsetof(struct acc_sqlite_region_entry_t, num_datas),
  offsetof(struct acc_sqlite_region_entry_t, opencl_file)
};

struct acc_sqlite_region_entry_t * acc_sqlite_get_region_entries(sqlite3 * db, size_t * count);

