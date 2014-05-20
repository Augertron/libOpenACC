
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/internal/compiler.h"

#include "OpenACC/utils/sqlite.h"
#include "OpenACC/utils/tuning.h"

#include "OpenACC/utils/containers/map.h"

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

#ifndef DBG_SQLITE_CONNECTION
# define DBG_SQLITE_CONNECTION 0
#endif

struct acc_sqlite_t_ {
  map_t db_files_map;
};

struct acc_sqlite_t_ * acc_sqlite;

void acc_sqlite_init() {
  if (acc_sqlite == NULL) {
    acc_sqlite = malloc(sizeof(struct acc_sqlite_t_));
    acc_sqlite->db_files_map = map_alloc(64, sizeof(sqlite3 *), sizeof(char *), NULL);
  }
}

void acc_sqlite_exit() {
  if (acc_sqlite != NULL) {
    map_free(acc_sqlite->db_files_map);
    free(acc_sqlite);
    acc_sqlite = NULL;
  }
}

const char * acc_sqlite_type_string(enum acc_sqlite_type_e type) {
  switch (type) {
    case e_sqlite_int:    return "INT";
    case e_sqlite_bigint: return "BIGINT";
    case e_sqlite_float:  return "FLOAT";
    case e_sqlite_text:   return "TEXT";
    default: assert(0);
  }
}

const size_t acc_sqlite_type_size(enum acc_sqlite_type_e type) {
  switch (type) {
    case e_sqlite_int:    return sizeof(size_t);
    case e_sqlite_bigint: return sizeof(long);
    case e_sqlite_float:  return sizeof(float);
    case e_sqlite_text:   return 64 * sizeof(char);
    default: assert(0);
  }
}

const char * acc_sqlite_type_format(enum acc_sqlite_type_e type) {
  switch (type) {
    case e_sqlite_int:    return "%zd";
    case e_sqlite_bigint: return "%ld";
    case e_sqlite_float:  return "%f";
    case e_sqlite_text:   return "%s";
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
      case e_sqlite_bigint:
      {
        long val = atol(values[i]);
        assert(user_data->field_sizes[i] == sizeof(long));
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

int acc_sqlite_load_or_save_db(sqlite3 * pInMemory, const char * zFilename, int isSave);

sqlite3 * acc_sqlite_open(char * filename_, int fail_if_file_missing, int use_in_memory_db) {
  acc_sqlite_init();

  assert(acc_sqlite != NULL);
#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_open_db(...)\n");
#endif

  struct stat buffer;
  int file_existed = stat(filename_, &buffer) == 0;

  if (fail_if_file_missing) assert(file_existed);

  sqlite3 * result = NULL;
  char * filename = NULL;
  if (use_in_memory_db == 1) {
    int status = sqlite3_open(":memory:", &result);
    assert(status == SQLITE_OK);

    filename = malloc((strlen(filename_) + 1) * sizeof(char));
    strcpy(filename, filename_);

    status = acc_sqlite_load_or_save_db(result, filename, 0);
    assert(status == SQLITE_OK);
  }
  else if (use_in_memory_db == 0) {
    int status = sqlite3_open(filename_, &result);
    assert(status == SQLITE_OK);
  }
  else assert(0);
  assert(result != NULL);

  map_insert(acc_sqlite->db_files_map, &result, &filename);

  return result;
}

void acc_sqlite_save(sqlite3 * db) {
  assert(acc_sqlite != NULL);
  char * filename = *(char**)map_lookup(acc_sqlite->db_files_map, &db);
  if (filename != NULL)
    acc_sqlite_load_or_save_db(db, filename, 1);
}

void acc_sqlite_reload(sqlite3 * db) {
  assert(acc_sqlite != NULL);
  char * filename = *(char**)map_lookup(acc_sqlite->db_files_map, &db);
  if (filename != NULL)
    acc_sqlite_load_or_save_db(db, filename, 0);
}

void acc_sqlite_close(sqlite3 * db) {
  assert(acc_sqlite != NULL);
  char * filename = *(char**)map_lookup(acc_sqlite->db_files_map, &db);
  if (filename != NULL) {
    acc_sqlite_load_or_save_db(db, filename, 1);
    free(filename);
  }
  map_remove(acc_sqlite->db_files_map, &db);
  sqlite3_close(db);
}

int acc_sqlite_table_exists(sqlite3 * db, char * table_name) {
#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_table_exists(...)\n");
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
#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_print_table(...)\n");
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
#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_build_select_query(...)\n");
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

#if DBG_SQLITE_CONNECTION
  printf("[debug]  Leave acc_sqlite_build_select_query, query = %s\n", *query);
#endif
}

size_t acc_sqlite_count_table_entries(sqlite3 * db, char * table_name, size_t num_conds, char ** conds) {
#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_count_table_entries(...)\n");
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
#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_read_table(...)\n");
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

void acc_sqlite_load_compiler_data(sqlite3 * db, struct acc_sqlite_load_compiler_data_filter_t_ * filter) {
  size_t i;

#if DBG_SQLITE_CONNECTION
  printf("[debug]  Enter acc_sqlite_load_compiler_data(...)\n");
#endif
  assert(compiler_data.num_regions == 0 && compiler_data.regions == NULL);

  assert(acc_sqlite_table_exists(db, "Regions"  ));
  assert(acc_sqlite_table_exists(db, "Kernels"  ));
  assert(acc_sqlite_table_exists(db, "Versions" ));
  assert(acc_sqlite_table_exists(db, "Loops"    ));

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
    region->num_devices = 1;          /// \todo add to Version DB
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
      kernel->num_loops = kernel_entries[k_idx].num_loops;
      kernel->splitted_loop = NULL; /// \todo add to Version DB
      kernel->version_by_devices = NULL;

      char kernel_id_cond[20];
      sprintf(kernel_id_cond, "kernel_id == '%zd'", kernel_entries[k_idx].kernel_id);

      size_t num_version_conds = filter != NULL ? 3 : 2;

      char ** version_conds = malloc(num_version_conds * sizeof(char*));
      version_conds[0] = region_id_cond;
      version_conds[1] = kernel_id_cond;

      struct acc_sqlite_parameter_entry_t * parameter_entries;
      kernel->num_params = acc_sqlite_read_table(
                             db, "Parameters", 2, version_conds,
                             parameter_entry_num_fields, parameter_entry_field_names, parameter_entry_field_types,
                                                         parameter_entry_field_sizes, parameter_entry_field_offsets,
                             sizeof(struct acc_sqlite_parameter_entry_t), (void**)&parameter_entries
                           );
      if (kernel->num_params > 0) {
        kernel->size_params = malloc(kernel->num_params * sizeof(size_t));
        for (i = 0; i < kernel->num_params; i++)
          kernel->size_params[parameter_entries[i].idx] = parameter_entries[i].size;
      }

      struct acc_sqlite_scalar_entry_t * scalar_entries;
      kernel->num_scalars = acc_sqlite_read_table(
                              db, "Scalars", 2, version_conds,
                              scalar_entry_num_fields, scalar_entry_field_names, scalar_entry_field_types,
                                                         scalar_entry_field_sizes, scalar_entry_field_offsets,
                              sizeof(struct acc_sqlite_scalar_entry_t), (void**)&scalar_entries
                            );
      if (kernel->num_scalars > 0) {
        kernel->size_scalars = malloc(kernel->num_scalars * sizeof(size_t));
        for (i = 0; i < kernel->num_scalars; i++)
          kernel->size_scalars[scalar_entries[i].idx] = scalar_entries[i].size;
      }

      struct acc_sqlite_data_entry_t * data_entries;
      kernel->num_datas = acc_sqlite_read_table(
                            db, "Datas", 2, version_conds,
                            data_entry_num_fields, data_entry_field_names, data_entry_field_types,
                                                   data_entry_field_sizes, data_entry_field_offsets,
                            sizeof(struct acc_sqlite_data_entry_t), (void**)&data_entries
                          );


      if (filter != NULL) {
        size_t num_enabled_versions = filter->num_enabled_versions[filter->region_offset[region_entries[r_idx].region_id] + kernel_entries[k_idx].kernel_id];
        size_t * enabled_versions = filter->enabled_versions[filter->region_offset[region_entries[r_idx].region_id] + kernel_entries[k_idx].kernel_id];

        version_conds[2] = malloc((24 * num_enabled_versions + 1) * sizeof(char));
        version_conds[2][0] = '\0';

        size_t i;
        char tmp_cond[25];
        sprintf(tmp_cond, "( version_id == '%zd'", enabled_versions[0]);
        strcat(version_conds[2], tmp_cond);
        for (i = 1; i < num_enabled_versions; i++) {
          strcat(version_conds[2], " OR ");
          sprintf(tmp_cond, "version_id == '%zd'", enabled_versions[i]);
          strcat(version_conds[2], tmp_cond);
        }
        strcat(version_conds[2], " )");
      }

      struct acc_sqlite_version_entry_t * version_entries;
      kernel->num_versions = acc_sqlite_read_table(
                               db, "Versions", num_version_conds, version_conds,
                               version_entry_num_fields, version_entry_field_names, version_entry_field_types,
                                                         version_entry_field_sizes, version_entry_field_offsets,
                               sizeof(struct acc_sqlite_version_entry_t), (void**)&version_entries
                             );
      assert(kernel->num_versions > 0);
      assert(kernel->num_versions <= kernel_entries[k_idx].num_versions);
      kernel->versions = malloc(kernel->num_versions * sizeof(struct acc_kernel_version_t_ *));

      for (v_idx = 0; v_idx < kernel->num_versions; v_idx++) {
        struct acc_kernel_version_t_ * version = malloc(sizeof(struct acc_kernel_version_t_));
        version->id = version_entries[v_idx].version_id;
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

#if DBG_SQLITE_CONNECTION
  printf("[debug]  Leave acc_sqlite_load_compiler_data\n");
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

size_t event_entry_num_fields = 8;
char * event_entry_field_names[8] = {
  "run_id", "device_id", "command_name", "command_id",
  "cl_profiling_command_queued", "cl_profiling_command_submit",
  "cl_profiling_command_start", "cl_profiling_command_end"
};
enum acc_sqlite_type_e event_entry_field_types[8] = {e_sqlite_int, e_sqlite_int, e_sqlite_text, e_sqlite_int, e_sqlite_bigint, e_sqlite_bigint, e_sqlite_bigint, e_sqlite_bigint};
size_t event_entry_field_sizes[8] = {sizeof(size_t), sizeof(size_t), sizeof(char[128]), sizeof(size_t), sizeof(long), sizeof(long), sizeof(long), sizeof(long)};
size_t event_entry_field_offsets[8] = {
  offsetof(struct acc_sqlite_event_entry_t, run_id),
  offsetof(struct acc_sqlite_event_entry_t, device_id),
  offsetof(struct acc_sqlite_event_entry_t, command_name),
  offsetof(struct acc_sqlite_event_entry_t, command_id),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_queued),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_submit),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_start),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_end)
};

size_t parameter_entry_num_fields = 4;
char * parameter_entry_field_names[4] = {"region_id", "kernel_id", "idx", "size"};
enum acc_sqlite_type_e parameter_entry_field_types[4] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t parameter_entry_field_sizes[4] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t parameter_entry_field_offsets[4] = {
  offsetof(struct acc_sqlite_parameter_entry_t, region_id),
  offsetof(struct acc_sqlite_parameter_entry_t, kernel_id),
  offsetof(struct acc_sqlite_parameter_entry_t, idx),
  offsetof(struct acc_sqlite_parameter_entry_t, size)
};

size_t scalar_entry_num_fields = 4;
char * scalar_entry_field_names[4] = {"region_id", "kernel_id", "idx", "size"};
enum acc_sqlite_type_e scalar_entry_field_types[4] = {e_sqlite_int, e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t scalar_entry_field_sizes[4] = {sizeof(size_t), sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t scalar_entry_field_offsets[4] = {
  offsetof(struct acc_sqlite_scalar_entry_t, region_id),
  offsetof(struct acc_sqlite_scalar_entry_t, kernel_id),
  offsetof(struct acc_sqlite_scalar_entry_t, idx),
  offsetof(struct acc_sqlite_scalar_entry_t, size)
};

size_t data_entry_num_fields = 3;
char * data_entry_field_names[3] = {"region_id", "kernel_id", "idx"};
enum acc_sqlite_type_e data_entry_field_types[3] = {e_sqlite_int, e_sqlite_int, e_sqlite_int};
size_t data_entry_field_sizes[3] = {sizeof(size_t), sizeof(size_t), sizeof(size_t)};
size_t data_entry_field_offsets[3] = {
  offsetof(struct acc_sqlite_data_entry_t, region_id),
  offsetof(struct acc_sqlite_data_entry_t, kernel_id),
  offsetof(struct acc_sqlite_data_entry_t, idx)
};

int acc_sqlite_read_run_table(
  sqlite3 * db,
  size_t num_devices,
  struct acc_tuner_data_params_desc_t_ * data_params,
  size_t num_conds, char ** conds,
  size_t * entry_size,
  void ** run_entries,
  size_t * num_fields,
  char *** field_names,
  enum acc_sqlite_type_e ** field_types,
  size_t ** field_sizes,
  size_t ** field_offsets,
  char * run_id_str
) {
  size_t i;
  size_t num_dev_fields = ((num_devices == 1) ? 5 : (num_devices * 6)) + 1;
  *num_fields = num_dev_fields + data_params->num_params;

  *field_names = malloc(*num_fields * sizeof(char *));
  *field_types = malloc(*num_fields * sizeof(enum acc_sqlite_type_e));
  *field_sizes = malloc(*num_fields * sizeof(size_t));
  *field_offsets = malloc(*num_fields * sizeof(size_t));

  if (run_id_str == NULL)
    (*field_names)  [0] = "rowid";
  else
    (*field_names)  [0] = run_id_str;
  (*field_types)  [0] = e_sqlite_int;
  (*field_sizes)  [0] = sizeof(size_t);
  (*field_offsets)[0] = 0;

  if (num_devices == 1) {
    (*field_names)  [1] = "version_id";
    (*field_types)  [1] = e_sqlite_int;
    (*field_sizes)  [1] = sizeof(size_t);
    (*field_offsets)[1] = (*field_offsets)[0] + (*field_sizes)[0];;

    (*field_names)  [2] = "acc_device_type";
    (*field_types)  [2] = e_sqlite_text;
    (*field_sizes)  [2] = sizeof(char[40]);
    (*field_offsets)[2] = (*field_offsets)[1] + (*field_sizes)[1];

    (*field_names)  [3] = "gang";
    (*field_types)  [3] = e_sqlite_int;
    (*field_sizes)  [3] = sizeof(size_t);
    (*field_offsets)[3] = (*field_offsets)[2] + (*field_sizes)[2];

    (*field_names)  [4] = "worker";
    (*field_types)  [4] = e_sqlite_int;
    (*field_sizes)  [4] = sizeof(size_t);
    (*field_offsets)[4] = (*field_offsets)[3] + (*field_sizes)[3];

    (*field_names)  [5] = "vector";
    (*field_types)  [5] = e_sqlite_int;
    (*field_sizes)  [5] = sizeof(size_t);
    (*field_offsets)[5] = (*field_offsets)[4] + (*field_sizes)[4];
  }
  else {
    size_t offset = 0;
    for (i = 0; i < num_devices; i++) {
      (*field_names)  [6 * i + 1] = malloc(14 * sizeof(char));
        sprintf((*field_names)[6 * i + 1], "version_id_%zd", i);
      (*field_types)  [6 * i + 1] = e_sqlite_int;
      (*field_sizes)  [6 * i + 1] = sizeof(size_t);
      (*field_offsets)[6 * i + 1] = offset;
      offset += (*field_sizes)[6 * i + 1];

      (*field_names)  [6 * i + 2] = malloc(19 * sizeof(char));
        sprintf((*field_names)[6 * i + 2], "acc_device_type_%zd", i);
      (*field_types)  [6 * i + 2] = e_sqlite_text;
      (*field_sizes)  [6 * i + 2] = sizeof(char[40]);
      (*field_offsets)[6 * i + 2] = offset;
      offset += (*field_sizes)[6 * i + 2];

      (*field_names)  [6 * i + 3] = malloc(8 * sizeof(char));
        sprintf((*field_names)[6 * i + 3], "gang_%zd", i);
      (*field_types)  [6 * i + 3] = e_sqlite_int;
      (*field_sizes)  [6 * i + 3] = sizeof(size_t);
      (*field_offsets)[6 * i + 3] = offset;
      offset += (*field_sizes)[6 * i + 3];

      (*field_names)  [6 * i + 4] = malloc(10 * sizeof(char));
        sprintf((*field_names)[6 * i + 4], "worker_%zd", i);
      (*field_types)  [6 * i + 4] = e_sqlite_int;
      (*field_sizes)  [6 * i + 4] = sizeof(size_t);
      (*field_offsets)[6 * i + 4] = offset;
      offset += (*field_sizes)[6 * i + 4];

      (*field_names)  [6 * i + 5] = malloc(10 * sizeof(char));
        sprintf((*field_names)[6 * i + 5], "vector_%zd", i);
      (*field_types)  [6 * i + 5] = e_sqlite_int;
      (*field_sizes)  [6 * i + 5] = sizeof(size_t);
      (*field_offsets)[6 * i + 5] = offset;
      offset += (*field_sizes)[6 * i + 5];

      (*field_names)  [6 * i + 6] = malloc(11 * sizeof(char));
        sprintf((*field_names)[6 * i + 6], "portion_%zd", i);
      (*field_types)  [6 * i + 6] = e_sqlite_int;
      (*field_sizes)  [6 * i + 6] = sizeof(size_t);
      (*field_offsets)[6 * i + 6] = offset;
      offset += (*field_sizes)[6 * i + 6];
    }
  }

  for (i = 0; i < data_params->num_params; i++) {
    (*field_names)  [num_dev_fields + i] = data_params->name_params[i];
    (*field_types)  [num_dev_fields + i] = data_params->type_params[i];
    (*field_sizes)  [num_dev_fields + i] = acc_sqlite_type_size(data_params->type_params[i]);
    (*field_offsets)[num_dev_fields + i] = (*field_offsets)[num_dev_fields + i - 1] + (*field_sizes)[num_dev_fields + i - 1];
  }

  *entry_size = 0;
  for (i = 0; i < *num_fields; i++)
    *entry_size += (*field_sizes)[i];

  return acc_sqlite_read_table(
    db, "Runs",
    num_conds, conds,
    *num_fields, *field_names, *field_types, *field_sizes, *field_offsets,
    *entry_size, run_entries
  );
}

void acc_sqlite_clean_run_table_read(
  size_t num_devices,
  char * field_names,
  enum acc_sqlite_type_e * field_types,
  size_t * field_sizes,
  size_t * field_offsets
) {
  size_t i;
  if (num_devices > 1)
    for (i = 0; i < num_devices; i++)
      free(field_names[6 * i + 1]);
  free(field_names);
  free(field_types);
  free(field_sizes);
  free(field_offsets);
}

/**
 * /brief From http://www.sqlite.org/backup.html
 *
** This function is used to load the contents of a database file on disk 
** into the "main" database of open database connection pInMemory, or
** to save the current contents of the database opened by pInMemory into
** a database file on disk. pInMemory is probably an in-memory database, 
** but this function will also work fine if it is not.
**
** Parameter zFilename points to a nul-terminated string containing the
** name of the database file on disk to load from or save to. If parameter
** isSave is non-zero, then the contents of the file zFilename are 
** overwritten with the contents of the database opened by pInMemory. If
** parameter isSave is zero, then the contents of the database opened by
** pInMemory are replaced by data loaded from the file zFilename.
**
** If the operation is successful, SQLITE_OK is returned. Otherwise, if
** an error occurs, an SQLite error code is returned.
*/
int acc_sqlite_load_or_save_db(sqlite3 * pInMemory, const char * zFilename, int isSave) {
  int rc;                   /* Function return code */
  sqlite3 *pFile;           /* Database connection opened on zFilename */
  sqlite3_backup *pBackup;  /* Backup object used to copy data */
  sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
  sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

  /* Open the database file identified by zFilename. Exit early if this fails
  ** for any reason. */
  rc = sqlite3_open(zFilename, &pFile);
  if( rc==SQLITE_OK ){

    /* If this is a 'load' operation (isSave==0), then data is copied
    ** from the database file just opened to database pInMemory. 
    ** Otherwise, if this is a 'save' operation (isSave==1), then data
    ** is copied from pInMemory to pFile.  Set the variables pFrom and
    ** pTo accordingly. */
    pFrom = (isSave ? pInMemory : pFile);
    pTo   = (isSave ? pFile     : pInMemory);

    /* Set up the backup procedure to copy from the "main" database of 
    ** connection pFile to the main database of connection pInMemory.
    ** If something goes wrong, pBackup will be set to NULL and an error
    ** code and  message left in connection pTo.
    **
    ** If the backup object is successfully created, call backup_step()
    ** to copy data from pFile to pInMemory. Then call backup_finish()
    ** to release resources associated with the pBackup object.  If an
    ** error occurred, then  an error code and message will be left in
    ** connection pTo. If no error occurred, then the error code belonging
    ** to pTo is set to SQLITE_OK.
    */
    pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
    if( pBackup ){
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
    }
    rc = sqlite3_errcode(pTo);
  }

  /* Close the database connection opened on database file zFilename
  ** and return the result of this function. */
  (void)sqlite3_close(pFile);
  return rc;
}

