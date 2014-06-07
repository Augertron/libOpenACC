
#ifndef __LIBOPENACC_SQLITE_REGION_DATA_H__
#define __LIBOPENACC_SQLITE_REGION_DATA_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_region_data_entry_t {
  size_t region_id;
  size_t data_id;
};
extern size_t region_data_entry_num_fields;
extern char * region_data_entry_field_names[2];
extern enum acc_sqlite_type_e region_data_entry_field_types[2];
extern size_t region_data_entry_field_sizes[2];
extern size_t region_data_entry_field_offsets[2];

struct acc_sqlite_region_data_entry_t * acc_sqlite_get_region_data_entries(sqlite3 * db, size_t * count, size_t region_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_REGION_DATA_H__ */

