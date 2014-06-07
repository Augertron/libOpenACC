
#ifndef __LIBOPENACC_SQLITE_REGION_PARAM_H__
#define __LIBOPENACC_SQLITE_REGION_PARAM_H__

#include "OpenACC/utils/sqlite/sqlite.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_region_parameter_entry_t {
  size_t region_id;
  size_t param_id;
  size_t size;
};
extern size_t region_parameter_entry_num_fields;
extern char * region_parameter_entry_field_names[3];
extern enum acc_sqlite_type_e region_parameter_entry_field_types[3];
extern size_t region_parameter_entry_field_sizes[3];
extern size_t region_parameter_entry_field_offsets[3];

struct acc_sqlite_region_parameter_entry_t * acc_sqlite_get_region_parameter_entries(sqlite3 * db, size_t * count, size_t region_id);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_REGION_PARAM_H__ */

