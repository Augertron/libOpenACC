	
#include "OpenACC/utils/sqlite.h"

#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/internal/compiler.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <assert.h>

#ifndef PRINT_INFO
# define PRINT_INFO 0
#endif

const char * acc_sqlite_type_string(enum acc_sqlite_type_e type) {
  switch (type) {
    case e_sqlite_int:   return "INT";
    case e_sqlite_float: return "FLOAT";
    case e_sqlite_text:  return "TEXT";
    default: assert(0);
  }
}

const size_t acc_sqlite_type_size(enum acc_sqlite_type_e type) {
  switch (type) {
    case e_sqlite_int:   return sizeof(size_t);
    case e_sqlite_float: return sizeof(float);
    case e_sqlite_text:  return 64 * sizeof(char);
    default: assert(0);
  }
}

static int acc_sqlite_callback_count_entries(void * user_data, int cnt, char ** values, char ** names) {
  (*(size_t*)user_data)++;
  return 0;
}

static int acc_sqlite_callback_print_table(void * user_data, int cnt, char ** values, char ** names){
  int i;
  for(i=0; i < cnt; i++){
    printf("%s = %s\n", names[i], values[i] ? values[i] : "NULL");
  }
  printf("\n");
  return 0;
}

struct acc_sqlite_user_data_save_entries_t {
  void * entries;
  enum acc_sqlite_type_e * field_types;
  size_t * field_sizes;
  size_t * field_offsets;
  size_t num_fields;
  size_t entry_size;
};

static int acc_sqlite_callback_save_entries(void * user_data_, int cnt, char ** values, char ** names) {
  struct acc_sqlite_user_data_save_entries_t * user_data = (struct acc_sqlite_user_data_save_entries_t *)user_data_;

  assert(user_data->num_fields == cnt);

  int i;
  for(i = 0; i < cnt; i++){
    switch (user_data->field_types[i]) {
      case e_sqlite_int:
      {
        size_t val = atoi(values[i]);
        assert(user_data->field_sizes[i] == sizeof(size_t));
        memcpy(user_data->entries + user_data->field_offsets[i], &val, user_data->field_sizes[i]);
        break;
      }
      case e_sqlite_float:
      {
        float val = atof(values[i]);
        assert(user_data->field_sizes[i] == sizeof(float));
        memcpy(user_data->entries + user_data->field_offsets[i], &val, user_data->field_sizes[i]);
        break;
      }
      case e_sqlite_text:
      {
        assert((strlen(values[i]) + 1) * sizeof(char) <= user_data->field_sizes[i]);
        strcpy(user_data->entries + user_data->field_offsets[i], values[i]);
        break;
      }
      default:
        assert(0);
    }
  }
  user_data->entries += user_data->entry_size;
  return 0;
}

sqlite3 * acc_sqlite_open_db(char * filename, int fail_if_file_missing) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_open_db(...)\n");
#endif
  sqlite3 * result;

  struct stat buffer;
  int file_existed = stat(filename, &buffer) == 0;

  if (fail_if_file_missing) assert(file_existed);

  int status = sqlite3_open(filename, &result);
  assert(status == SQLITE_OK);

  return result;
}

int acc_sqlite_table_exists(sqlite3 * db, char * table_name) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_table_exists(...)\n");
#endif
  assert(db != NULL && table_name != NULL);

  size_t cnt = 0;
  char * err_msg;
  char * query = malloc((60 + strlen(table_name)) * sizeof(char));
  sprintf(query, "SELECT * FROM sqlite_master WHERE name='%s' and type='table';", table_name);
  int status = sqlite3_exec (db, query, &acc_sqlite_callback_count_entries, &cnt, &err_msg);
  assert(status == SQLITE_OK);
  free(query);

  assert(cnt == 0 || cnt == 1);

  return cnt;
}

void acc_sqlite_print_table(sqlite3 * db, char * table_name) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_print_table(...)\n");
#endif
  assert(db != NULL && table_name != NULL);

  char * err_msg;
  char * query = malloc((16 + strlen(table_name)) * sizeof(char));
  sprintf(query, "SELECT * FROM %s;", table_name);
  int status = sqlite3_exec (db, query, &acc_sqlite_callback_print_table, NULL, &err_msg);
  assert(status == SQLITE_OK);
  free(query);
}

int acc_sqlite_create_table(sqlite3 * db, char * table_name, char * table_content) {
  assert(db != NULL && table_name != NULL && table_content != NULL);

  if (acc_sqlite_table_exists(db, table_name)) return 0;

  char * err_msg;
  char * query = malloc((20 + strlen(table_name) + strlen(table_content)) * sizeof(char));
  sprintf(query, "CREATE TABLE %s ( %s );", table_name, table_content);
  int status = sqlite3_exec (db, query, NULL, 0, &err_msg);
  assert(status == SQLITE_OK);
  free(query);

  return 1;
}

void acc_sqlite_build_select_query(char * table_name, size_t num_fields, char ** fields, size_t num_conds, char ** conds, char ** query) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_build_select_query(...)\n");
#endif
  assert(table_name != NULL && (num_conds == 0 || conds != NULL) && (num_fields == 0 || fields != NULL));

  size_t i;

  {
    size_t query_length = 15 + strlen(table_name);

    if (num_fields > 0) {
      query_length += 2 * (num_fields - 1);
      for (i = 0; i < num_fields; i++)
        query_length += strlen(fields[i]);
    }
    else query_length += 1;

    if (num_conds > 0) {
      query_length += 7 + 5 * (num_conds - 1);
      for (i = 0; i < num_conds; i++)
        query_length += strlen(conds[i]);
    }

    *query = malloc(query_length * sizeof(char));
    (*query)[0] = '\0';
  }

  strcat(*query, "SELECT ");

  if (num_fields > 0) {
    strcat(*query, fields[0]);
    for (i = 1; i < num_fields; i++) {
      strcat(*query, ", ");
      strcat(*query, fields[i]);
    }
  }
  else
    strcat(*query, "*");

  strcat(*query, " FROM ");
  strcat(*query, table_name);

  if (num_conds > 0) {
    strcat(*query, " WHERE ");
    strcat(*query, conds[0]);
    for (i = 1; i < num_conds; i++) {
      strcat(*query, " AND ");
      strcat(*query, conds[i]);
    }
  }
  strcat(*query, ";");

#if PRINT_INFO
  printf("[info]   Leave acc_sqlite_build_select_query, query = %s\n", *query);
#endif
}

size_t acc_sqlite_count_table_entries(sqlite3 * db, char * table_name, size_t num_conds, char ** conds) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_count_table_entries(...)\n");
#endif
  assert(db != NULL && table_name != NULL && (num_conds == 0 || conds != NULL));

  char * query;
  acc_sqlite_build_select_query(table_name, 0, NULL, num_conds, conds, &query);

  size_t cnt = 0;
  char * err_msg;
  int status = sqlite3_exec (db, query, &acc_sqlite_callback_count_entries, &cnt, &err_msg);
  assert(status == SQLITE_OK);
  free(query);

  return cnt;
}

size_t acc_sqlite_read_table(
  sqlite3 * db, char * table_name,
  size_t num_conds, char ** conds,
  size_t num_fields, char ** field_names, enum acc_sqlite_type_e * field_types, size_t * field_sizes, size_t * field_offsets,
  size_t entry_size, void ** entries
) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_read_table(...)\n");
#endif
  size_t num_entries = acc_sqlite_count_table_entries(db, table_name, num_conds, conds);
  *entries = malloc(num_entries * entry_size);

  char * query;
  acc_sqlite_build_select_query(table_name, num_fields, field_names, num_conds, conds, &query);

  struct acc_sqlite_user_data_save_entries_t * user_data = malloc(sizeof(struct acc_sqlite_user_data_save_entries_t));
    user_data->entries = *entries;
    user_data->field_types = field_types;
    user_data->field_sizes = field_sizes;
    user_data->field_offsets = field_offsets;
    user_data->num_fields = num_fields;
    user_data->entry_size = entry_size;

  char * err_msg;
  int status = sqlite3_exec (db, query, &acc_sqlite_callback_save_entries, user_data, &err_msg);
  assert(status == SQLITE_OK);
  free(query);

  return num_entries;
}

void acc_sqlite_load_compiler_data(sqlite3 * db) {
#if PRINT_INFO
  printf("[info]   Enter acc_sqlite_load_compiler_data(...)\n");
#endif
  assert(compiler_data.num_regions == 0 && compiler_data.regions == NULL);

  acc_sqlite_table_exists(db, "Regions");
  acc_sqlite_table_exists(db, "Kernels");
  acc_sqlite_table_exists(db, "Versions");
  acc_sqlite_table_exists(db, "Loops");

  struct acc_sqlite_region_entry_t * region_entries;
  compiler_data.num_regions = acc_sqlite_read_table(
                                db, "Regions", 0, NULL,
                                region_entry_num_fields, region_entry_field_names, region_entry_field_types,
                                                         region_entry_field_sizes, region_entry_field_offsets,
                                sizeof(struct acc_sqlite_region_entry_t), (void**)&region_entries
                              );

  size_t r_idx, k_idx, v_idx, l_idx;

  compiler_data.regions = malloc(compiler_data.num_regions * sizeof(struct acc_region_desc_t_ *));

  for (r_idx = 0; r_idx < compiler_data.num_regions; r_idx++) {
    struct acc_region_desc_t_ * region = malloc(sizeof(struct acc_region_desc_t_));
    region->id = region_entries[r_idx].region_id;
    region->file = malloc((strlen(region_entries[r_idx].opencl_file) + 1) * sizeof(char));
      strcpy(region->file, region_entries[r_idx].opencl_file);
    region->num_options = 0;          /// \todo add to Version DB
    region->options = NULL;           /// \todo add to Version DB
    region->num_devices = 0;          /// \todo add to Version DB
    region->devices = NULL;           /// \todo add to Version DB
    region->num_distributed_data = 0; /// \todo add to Version DB
    region->distributed_data = NULL;  /// \todo add to Version DB

    char region_id_cond[20];
    sprintf(region_id_cond, "region_id == '%zd'", region_entries[r_idx].region_id);
    char * kernel_conds[1] = {region_id_cond};

    struct acc_sqlite_kernel_entry_t * kernel_entries;
    region->num_kernels = acc_sqlite_read_table(
                            db, "Kernels", 1, kernel_conds,
                            kernel_entry_num_fields, kernel_entry_field_names, kernel_entry_field_types,
                                                     kernel_entry_field_sizes, kernel_entry_field_offsets,
                            sizeof(struct acc_sqlite_kernel_entry_t), (void**)&kernel_entries
                          );
    assert(region->num_kernels == region_entries[r_idx].num_kernels);
    region->kernels = malloc(region->num_kernels * sizeof(struct acc_kernel_desc_t_ *));

    for (k_idx = 0; k_idx < region->num_kernels; k_idx++) {
      struct acc_kernel_desc_t_ * kernel = malloc(sizeof(struct acc_kernel_desc_t_));
      kernel->id = kernel_entries[k_idx].kernel_id;
      kernel->name = malloc((strlen(kernel_entries[k_idx].name) + 1) * sizeof(char));
        strcpy(kernel->name, kernel_entries[k_idx].name);
      kernel->num_params = 0;       /// \todo add to Version DB
      kernel->size_params = NULL;   /// \todo add to Version DB
      kernel->num_scalars = 0;      /// \todo add to Version DB
      kernel->size_scalars = NULL;  /// \todo add to Version DB
      kernel->num_datas = 0;        /// \todo add to Version DB
      kernel->num_loops = kernel_entries[k_idx].num_loops;
      kernel->splitted_loop = NULL; /// \todo add to Version DB
      kernel->version_by_devices = NULL;

      char kernel_id_cond[20];
      sprintf(kernel_id_cond, "kernel_id == '%zd'", kernel_entries[r_idx].kernel_id);
      char * version_conds[2] = {region_id_cond, kernel_id_cond};

      struct acc_sqlite_version_entry_t * version_entries;
      kernel->num_versions = acc_sqlite_read_table(
                               db, "Versions", 2, version_conds,
                               version_entry_num_fields, version_entry_field_names, version_entry_field_types,
                                                         version_entry_field_sizes, version_entry_field_offsets,
                               sizeof(struct acc_sqlite_version_entry_t), (void**)&version_entries
                             );
      assert(kernel->num_versions == kernel_entries[k_idx].num_versions);
      kernel->versions = malloc(kernel->num_versions * sizeof(struct acc_kernel_version_t_ *));

      for (v_idx = 0; v_idx < kernel->num_versions; v_idx++) {
        struct acc_kernel_version_t_ * version = malloc(sizeof(struct acc_kernel_version_t_));
        version->num_gang = 0;      /// \todo add to Version DB
        version->num_worker = 0;    /// \todo add to Version DB
        version->vector_length = 1; /// \todo add to Version DB
        version->suffix = malloc((strlen(version_entries[v_idx].suffix) + 1) * sizeof(char));;
          strcpy(version->suffix, version_entries[v_idx].suffix);
        version->device_affinity = acc_device_any;

        char version_id_cond[20];
        sprintf(version_id_cond, "version_id == '%zd'", version_entries[v_idx].version_id);
        char * loop_conds[3] = {region_id_cond, kernel_id_cond, version_id_cond};

        struct acc_sqlite_loop_entry_t * loop_entries;
        size_t num_loops = acc_sqlite_read_table(
                             db, "Loops",
                             3, loop_conds,
                             loop_entry_num_fields, loop_entry_field_names, loop_entry_field_types, loop_entry_field_sizes, loop_entry_field_offsets,
                             sizeof(struct acc_sqlite_loop_entry_t), (void**)&loop_entries
                           );
        //assert(num_loops == version->num_loops); /// \todo insert valid value into Version DB
        version->loops = malloc(num_loops * sizeof(struct acc_loop_t_));

        for (l_idx = 0; l_idx < num_loops; l_idx++) {
          version->loops[l_idx].num_iterations[0] = loop_entries[l_idx].tiles[0];
          version->loops[l_idx].num_iterations[1] = loop_entries[l_idx].tiles[1];
          version->loops[l_idx].num_iterations[2] = loop_entries[l_idx].tiles[2];
          version->loops[l_idx].num_iterations[3] = loop_entries[l_idx].tiles[3];
          version->loops[l_idx].num_iterations[4] = loop_entries[l_idx].tiles[4];
          version->loops[l_idx].num_iterations[5] = loop_entries[l_idx].tiles[5];
          version->loops[l_idx].num_iterations[6] = loop_entries[l_idx].tiles[6];
        }
        kernel->versions[v_idx] = version;
      }
      region->kernels[k_idx] = kernel;
    }
    compiler_data.regions[r_idx] = region;
  }

  free(region_entries);

#if PRINT_INFO
  printf("[info]   Leave acc_sqlite_load_compiler_data\n");
#endif
}

size_t region_entry_num_fields = 3;
char * region_entry_field_names[3] = {"region_id", "num_kernels", "opencl_file"};
enum acc_sqlite_type_e region_entry_field_types[3] = {e_sqlite_int, e_sqlite_int, e_sqlite_text};
size_t region_entry_field_sizes[3] = {sizeof(size_t), sizeof(size_t), sizeof(char[50])};
size_t region_entry_field_offsets[3] = {offsetof(struct acc_sqlite_region_entry_t, region_id), offsetof(struct acc_sqlite_region_entry_t, num_kernels), offsetof(struct acc_sqlite_region_entry_t, opencl_file)};

size_t kernel_entry_num_fields = 5;
char * kernel_entry_field_names[5] = {"region_id", "kernel_id", "name", "num_versions", "num_loops"};
enum acc_sqlite_type_e kernel_entry_field_types[5] = {e_sqlite_int, e_sqlite_int, e_sqlite_text, e_sqlite_int, e_sqlite_int};
size_t kernel_entry_field_sizes[5] = {sizeof(size_t), sizeof(size_t), sizeof(char[30]), sizeof(size_t), sizeof(size_t)};
size_t kernel_entry_field_offsets[5] = {
  offsetof(struct acc_sqlite_kernel_entry_t, region_id),
  offsetof(struct acc_sqlite_kernel_entry_t, kernel_id),
  offsetof(struct acc_sqlite_kernel_entry_t, name),
  offsetof(struct acc_sqlite_kernel_entry_t, num_versions),
  offsetof(struct acc_sqlite_kernel_entry_t, num_loops)
};

size_t version_entry_num_fields = 4;
char * version_entry_field_names[4] = {"region_id", "kernel_id", "version_id", "suffix"};
enum acc_sqlite_type_e version_entry_field_types[4] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_text};
size_t version_entry_field_sizes[4] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(char[30])};
size_t version_entry_field_offsets[4] = {
  offsetof(struct acc_sqlite_version_entry_t, region_id),
  offsetof(struct acc_sqlite_version_entry_t, kernel_id),
  offsetof(struct acc_sqlite_version_entry_t, version_id),
  offsetof(struct acc_sqlite_version_entry_t, suffix)
};

size_t loop_entry_num_fields = 15;
char * loop_entry_field_names[15] = {
  "region_id", "kernel_id", "version_id", "loop_id",
  "tile_0", "gang", "tile_1", "worker", "tile_2", "vector", "tile_3",
  "unroll_tile_0", "unroll_tile_1", "unroll_tile_2", "unroll_tile_3"
};
enum acc_sqlite_type_e loop_entry_field_types[15] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t loop_entry_field_sizes[15] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t loop_entry_field_offsets[15] = {
  offsetof(struct acc_sqlite_loop_entry_t, region_id),
  offsetof(struct acc_sqlite_loop_entry_t, kernel_id),
  offsetof(struct acc_sqlite_loop_entry_t, version_id),
  offsetof(struct acc_sqlite_loop_entry_t, loop_id),
  offsetof(struct acc_sqlite_loop_entry_t, tiles),
  offsetof(struct acc_sqlite_loop_entry_t, tiles) + sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, tiles) + 2 * sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, tiles) + 3 * sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, tiles) + 4 * sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, tiles) + 5 * sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, tiles) + 6 * sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, unroll),
  offsetof(struct acc_sqlite_loop_entry_t, unroll) + sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, unroll) + 2 * sizeof(size_t),
  offsetof(struct acc_sqlite_loop_entry_t, unroll) + 3 * sizeof(size_t)
};

