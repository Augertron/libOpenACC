
#ifndef __LIBOPENACC_SQLITE_REGION_H__
#define __LIBOPENACC_SQLITE_REGION_H__

#include "OpenACC/utils/sqlite/sqlite.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_region_entry_t {
  size_t region_id;
  size_t num_loops;
  size_t num_params;
  size_t num_scalars;
  size_t num_datas;
  char opencl_file[50];
};
extern size_t region_entry_num_fields;
extern char * region_entry_field_names[6];
extern enum acc_sqlite_type_e region_entry_field_types[6];
extern size_t region_entry_field_sizes[6];
extern size_t region_entry_field_offsets[6];

struct acc_sqlite_region_entry_t * acc_sqlite_get_region_entries(sqlite3 * db, size_t * count);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_REGION_H__ */

