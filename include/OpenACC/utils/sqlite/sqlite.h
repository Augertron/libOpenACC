
#ifndef __LIBOPENACC_SQLITE_H__
#define __LIBOPENACC_SQLITE_H__

#include "OpenACC/openacc.h"
#include "OpenACC/utils/containers/map.h"
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

void acc_sqlite_init();

void acc_sqlite_exit();

enum acc_sqlite_type_e {
  e_sqlite_int,
  e_sqlite_bigint,
  e_sqlite_float,
  e_sqlite_text
};

const char * acc_sqlite_type_string(enum acc_sqlite_type_e type);
const size_t acc_sqlite_type_size  (enum acc_sqlite_type_e type);
const char * acc_sqlite_type_format(enum acc_sqlite_type_e type);

sqlite3 * acc_sqlite_open(char * filename, int fail_if_file_missing, int use_in_memory_db);
void acc_sqlite_save  (sqlite3 * db);
void acc_sqlite_reload(sqlite3 * db);
void acc_sqlite_close (sqlite3 * db);

int acc_sqlite_table_exists(sqlite3 * db, char * table_name);

void acc_sqlite_print_table(sqlite3 * db, char * table_name);

int acc_sqlite_create_table(sqlite3 * db, char * table_name, char * table_content);

size_t acc_sqlite_count_table_entries(sqlite3 * db, char * table_name, size_t num_conds, char ** conds);

size_t acc_sqlite_read_table(
  sqlite3 * db, char * table_name,
  size_t num_conds, char ** conds,
  size_t num_fields, char ** field_names, enum acc_sqlite_type_e * field_types,
                     size_t * field_sizes, size_t * field_offsets,
  size_t entry_size, void ** entries
);

struct acc_sqlite_load_compiler_data_filter_t_ {
  size_t ** enabled_versions;
  size_t * num_enabled_versions;
  size_t * region_offset;
};

void acc_sqlite_load_compiler_data(sqlite3 * db, struct acc_sqlite_load_compiler_data_filter_t_ * filter);

#ifdef __cplusplus
}
#endif


#endif /* __LIBOPENACC_SQLITE_H__ */

