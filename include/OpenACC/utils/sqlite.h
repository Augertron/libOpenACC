
#ifndef __LIBOPENACC_SQLITE_H__
#define __LIBOPENACC_SQLITE_H__

#include "OpenACC/openacc.h"

#include "sqlite3.h"
#include "OpenACC/utils/containers/map.h"

#ifdef __cplusplus
extern "C" {
#endif

void acc_sqlite_init();

void acc_sqlite_exit();

enum acc_sqlite_type_e {
  e_sqlite_int,
  e_sqlite_float,
  e_sqlite_text
};

const char * acc_sqlite_type_string(enum acc_sqlite_type_e type);
const size_t acc_sqlite_type_size  (enum acc_sqlite_type_e type);

struct acc_sqlite_region_entry_t {
  size_t region_id;
  size_t num_kernels;
  char opencl_file[50];
};
extern size_t region_entry_num_fields;
extern char * region_entry_field_names[3];
extern enum acc_sqlite_type_e region_entry_field_types[3];
extern size_t region_entry_field_sizes[3];
extern size_t region_entry_field_offsets[3];

struct acc_sqlite_kernel_entry_t {
  size_t region_id;
  size_t kernel_id;
  char name[30];
  size_t num_versions;
  size_t num_loops;
};
extern size_t kernel_entry_num_fields;
extern char * kernel_entry_field_names[5];
extern enum acc_sqlite_type_e kernel_entry_field_types[5];
extern size_t kernel_entry_field_sizes[5];
extern size_t kernel_entry_field_offsets[5];

struct acc_sqlite_version_entry_t {
  size_t region_id;
  size_t kernel_id;
  size_t version_id;
  char suffix[30];
};
extern size_t version_entry_num_fields;
extern char * version_entry_field_names[4];
extern enum acc_sqlite_type_e version_entry_field_types[4];
extern size_t version_entry_field_sizes[4];
extern size_t version_entry_field_offsets[4];

struct acc_sqlite_loop_entry_t {
  size_t region_id;
  size_t kernel_id;
  size_t version_id;
  size_t loop_id;
  size_t tiles[7];
  size_t unroll[4];
};
extern size_t loop_entry_num_fields;
extern char * loop_entry_field_names[15];
extern enum acc_sqlite_type_e loop_entry_field_types[15];
extern size_t loop_entry_field_sizes[15];
extern size_t loop_entry_field_offsets[15];;

sqlite3 * acc_sqlite_open(char * filename, int fail_if_file_missing);
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

