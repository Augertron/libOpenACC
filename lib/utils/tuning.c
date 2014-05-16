
#include "OpenACC/private/debug.h"
#include "OpenACC/private/region.h"
#include "OpenACC/private/kernel.h"
#include "OpenACC/private/loop.h"
#include "OpenACC/private/data-env.h"
#include "OpenACC/private/memory.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/init.h"

#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/internal/compiler.h"

#include "OpenACC/utils/tuning.h"
#include "OpenACC/utils/profiling.h"
#include "OpenACC/utils/sqlite.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>

#ifndef PRINT_INFO
# define PRINT_INFO 0
#endif

struct acc_tuner_t * acc_tuner;

struct acc_tuner_device_param_t {
  char device_name[40];
  size_t version_id;
  size_t gang;
  size_t worker;
  size_t vector;
  size_t portion;
};

/**
 *  Register an experiment description with 'acc_profiling_set_experiment'
 */
int acc_tuning_create_experiment();

/**
 *  Register a run as part of the experiment with 'acc_profiling_new_run'
 */
void acc_tuning_add_run(struct acc_tuner_device_param_t * device_params, void * params);

void acc_tuning_init(
  size_t num_devices,
  char ** devices_name,
  struct acc_tuner_data_params_desc_t_ * data_params,
  sqlite3 * versions_db
) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_init(...)\n");
#endif
  assert(num_devices > 0);
  assert(devices_name != NULL);
  assert(data_params != NULL);
  assert(versions_db != NULL);

  acc_tuner = (struct acc_tuner_t *)malloc(sizeof(struct acc_tuner_t));

  acc_tuner->num_devices = num_devices;
  acc_tuner->devices_name = devices_name;

  acc_tuner->data_params = data_params;

  size_t i;
  acc_tuner->params_size = 0;
  for (i = 0; i < acc_tuner->data_params->num_params; i++)
    acc_tuner->params_size += acc_sqlite_type_size(acc_tuner->data_params->type_params[i]);

  acc_tuner->versions_db = versions_db;
}

struct acc_tuner_gen_data_t * acc_tuning_build_gen_data() {
  assert(acc_tuner != NULL);

  struct acc_tuner_gen_data_t * result = malloc(sizeof(struct acc_tuner_gen_data_t));

  result->region_id = 0;
  result->kernel_id = 0;

  result->per_devices_gen_data = malloc(acc_tuner->num_devices * sizeof(struct acc_tuner_per_devices_gen_data_t));

  size_t i;
  for (i = 0; i < acc_tuner->num_devices; i++) {
    result->per_devices_gen_data[i].num_versions = 0;
    result->per_devices_gen_data[i].version_ids = NULL;

    result->per_devices_gen_data[i].num_gang_values = 0;
    result->per_devices_gen_data[i].gang_values = NULL;

    result->per_devices_gen_data[i].num_worker_values = 0;
    result->per_devices_gen_data[i].worker_values = NULL;

    result->per_devices_gen_data[i].num_vector_values = 0;
    result->per_devices_gen_data[i].vector_values = NULL;
  }

  result->num_params_values = 0;
  result->params_values = NULL;

  result->num_portions = 0;

  result->loop_length_func = NULL;

  return result;
}

void acc_tuning_gen_all_version_combinaisons(size_t * version_combinaisons, size_t * version_combinaison_idx, size_t dev_idx, size_t * buffer, struct acc_tuner_gen_data_t * gen_data) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_gen_all_version_combinaisons(dev_idx = %zd)\n", dev_idx);
#endif
  if (dev_idx < acc_tuner->num_devices) {
    size_t version_idx;
    for (version_idx = 0; version_idx < gen_data->per_devices_gen_data[dev_idx].num_versions; version_idx++) {
      buffer[dev_idx] = gen_data->per_devices_gen_data[dev_idx].version_ids[version_idx];
      acc_tuning_gen_all_version_combinaisons(version_combinaisons, version_combinaison_idx, dev_idx + 1, buffer, gen_data);
    }
  }
  else {
#if PRINT_INFO
  printf("[info]   Save... (*version_combinaison_idx = %zd)\n", *version_combinaison_idx);
#endif
    memcpy(version_combinaisons + (*version_combinaison_idx) * acc_tuner->num_devices, buffer, acc_tuner->num_devices * sizeof(size_t));
    (*version_combinaison_idx)++;
  }
}

size_t acc_tuning_count_portion_combinaisons(size_t portions, size_t num_dev) {
  if (portions == 0)  return 1;
  if (num_dev  == 1)  return 1;

  size_t res = 0;
  size_t i;
  for (i = 0; i <= portions; i++)
    res += acc_tuning_count_portion_combinaisons(i, num_dev - 1);
  return res;
}

void acc_tuning_gen_all_portion_combinaisons(size_t portion, size_t dev_idx, size_t * portion_combinaisons, size_t * portion_combinaison_idx, size_t * buffer) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_gen_all_portion_combinaisons(portion = %zd, dev_idx = %zd)\n", portion, dev_idx);
#endif
  if (dev_idx < acc_tuner->num_devices - 1) {
    size_t i;
    for (i = 0; i <= portion; i++) {
      buffer[dev_idx] = i;
      acc_tuning_gen_all_portion_combinaisons(portion - i, dev_idx + 1, portion_combinaisons, portion_combinaison_idx, buffer);
    }
  }
  else if (dev_idx == acc_tuner->num_devices - 1) {
#if PRINT_INFO
  printf("[info]   Save... (*portion_combinaison_idx = %zd)\n", *portion_combinaison_idx);
#endif
    buffer[dev_idx] = portion;
    memcpy(portion_combinaisons + (*portion_combinaison_idx) * acc_tuner->num_devices, buffer, acc_tuner->num_devices * sizeof(size_t));
    (*portion_combinaison_idx)++;
  }
  else assert(0);
}

struct acc_tuner_gwv_list_t_ {
  size_t num;
  size_t (*gwv)[3];
};

int acc_tuner_is_valid_gwv_combinaison(
  void * params,
  size_t num_loops,
  struct acc_sqlite_loop_entry_t * loop_entries,
  size_t portion,
  size_t gwv[3],
  struct acc_tuner_gen_data_t * gen_data
) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_is_valid_gwv_combinaison(...)\n");
#endif
  size_t i;
  for (i = 0; i < num_loops; i++) {
    if ((loop_entries[i].tiles[1] == 1) && (loop_entries[i].tiles[3] == 1) && (loop_entries[i].tiles[5] == 1)) continue;

    size_t loop_length = (*(gen_data->loop_length_func))(i, params);
    int has_dynamic_tile = (loop_entries[i].tiles[0] == 0) ||
                           (loop_entries[i].tiles[2] == 0) ||
                           (loop_entries[i].tiles[4] == 0) ||
                           (loop_entries[i].tiles[6] == 0);
    size_t length = 1;
      length *= loop_entries[i].tiles[0] == 0 ? 1 : loop_entries[i].tiles[0];
      length *= loop_entries[i].tiles[2] == 0 ? 1 : loop_entries[i].tiles[2];
      length *= loop_entries[i].tiles[4] == 0 ? 1 : loop_entries[i].tiles[4];
      length *= loop_entries[i].tiles[6] == 0 ? 1 : loop_entries[i].tiles[6];

    if (loop_entries[i].tiles[1] == 0)
      length *= gwv[0];
    else
      assert(loop_entries[i].tiles[1] == 1);

    if (loop_entries[i].tiles[3] == 0)
      length *= gwv[1];
    else
      assert(loop_entries[i].tiles[3] == 1);

    if (loop_entries[i].tiles[5] == 0)
      length *= gwv[2];
    else
      assert(loop_entries[i].tiles[5] == 1);

    if (length > loop_length || (!has_dynamic_tile && length != loop_length))
      return 0;
  }

  return 1;
}

void acc_tuning_gen_valid_gwv_combinaisons_rec(
  void * params,
  size_t * versions,
  size_t * portions,
  size_t * num_gwv_combinaisons,
  size_t (**gwv_combinaisons)[3],
  size_t * size_gwv_combinaisons,
  struct acc_tuner_gwv_list_t_ * gwv_lists,
  size_t dev_idx, size_t (*buffer)[3],
  size_t * num_loops,
  struct acc_sqlite_loop_entry_t ** loop_entries,
  struct acc_tuner_gen_data_t * gen_data
) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_gen_valid_gwv_combinaisons_rec(...)\n");
#endif
  if (dev_idx < acc_tuner->num_devices) {
    size_t version = versions[dev_idx];
    size_t portion = portions[dev_idx];

    size_t gwv_idx;
    for (gwv_idx = 0; gwv_idx < gwv_lists[dev_idx].num; gwv_idx++) {
      if (!acc_tuner_is_valid_gwv_combinaison(params, num_loops[dev_idx], loop_entries[dev_idx], portion, gwv_lists[dev_idx].gwv[gwv_idx], gen_data)) continue;

      memcpy(buffer[dev_idx], gwv_lists[dev_idx].gwv[gwv_idx], sizeof(size_t[3]));

      acc_tuning_gen_valid_gwv_combinaisons_rec(
        params, versions, portions,
        num_gwv_combinaisons,
        gwv_combinaisons,
        size_gwv_combinaisons,
        gwv_lists, dev_idx + 1,
        buffer, num_loops, loop_entries,
        gen_data
      );
    }
  }
  else {
    memcpy(*gwv_combinaisons + *num_gwv_combinaisons * acc_tuner->num_devices, buffer, acc_tuner->num_devices * sizeof(size_t[3]));
    (*num_gwv_combinaisons)++;
    if (*num_gwv_combinaisons == *size_gwv_combinaisons) {
      *size_gwv_combinaisons *= 2;
      *gwv_combinaisons = realloc(*gwv_combinaisons, *size_gwv_combinaisons * acc_tuner->num_devices * sizeof(size_t[3]));
    }
  }
}

void acc_tuning_gen_valid_gwv_combinaisons(
  void * params,
  size_t * versions,
  size_t * portions,
  size_t * num_gwv_combinaisons,
  size_t (**gwv_combinaisons)[3],
  size_t * num_loops,
  struct acc_sqlite_loop_entry_t ** loop_entries,
  struct acc_tuner_gen_data_t * gen_data
) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_gen_valid_gwv_combinaisons(...)\n");
#endif
  *num_gwv_combinaisons = 0;

  size_t size_gwv_combinaisons = 64;
  *gwv_combinaisons = malloc(size_gwv_combinaisons * acc_tuner->num_devices * sizeof(size_t[3]));

  struct acc_tuner_gwv_list_t_ * gwv_lists = (struct acc_tuner_gwv_list_t_ *)malloc(acc_tuner->num_devices * sizeof(struct acc_tuner_gwv_list_t_));
  size_t dev_idx, g_idx, w_idx, v_idx, cnt;
  for (dev_idx = 0; dev_idx < acc_tuner->num_devices; dev_idx++) {
    gwv_lists[dev_idx].num = gen_data->per_devices_gen_data[dev_idx].num_gang_values
                           * gen_data->per_devices_gen_data[dev_idx].num_worker_values
                           * gen_data->per_devices_gen_data[dev_idx].num_vector_values;
    gwv_lists[dev_idx].gwv = malloc(gwv_lists[dev_idx].num * sizeof(size_t[3]));
    cnt = 0;
    for (g_idx = 0; g_idx < gen_data->per_devices_gen_data[dev_idx].num_gang_values; g_idx++) {
      for (w_idx = 0; w_idx < gen_data->per_devices_gen_data[dev_idx].num_worker_values; w_idx++) {
        for (v_idx = 0; v_idx < gen_data->per_devices_gen_data[dev_idx].num_vector_values; v_idx++) {
          gwv_lists[dev_idx].gwv[cnt][0] = gen_data->per_devices_gen_data[dev_idx].gang_values[g_idx];
          gwv_lists[dev_idx].gwv[cnt][1] = gen_data->per_devices_gen_data[dev_idx].worker_values[w_idx];
          gwv_lists[dev_idx].gwv[cnt][2] = gen_data->per_devices_gen_data[dev_idx].vector_values[v_idx];
          cnt++;
        }
      }
    }
  }

  size_t (*buffer)[3] = malloc(acc_tuner->num_devices * sizeof(size_t[3]));
  acc_tuning_gen_valid_gwv_combinaisons_rec(
    params, versions, portions,
    num_gwv_combinaisons,
    gwv_combinaisons,
    &size_gwv_combinaisons,
    gwv_lists, 0, buffer,
    num_loops, loop_entries,
    gen_data
  );
  free(buffer);
}


void acc_tuning_generate(struct acc_tuner_gen_data_t * gen_data) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_generate(...)\n");
#endif
  assert(acc_tuner != NULL);

  size_t dev_idx;

  size_t * buffer = (size_t *)malloc(acc_tuner->num_devices * sizeof(size_t));

  // Generate all combinaison of versions

  size_t num_version_combinaisons = 1;
  for (dev_idx = 0; dev_idx < acc_tuner->num_devices; dev_idx++)
    num_version_combinaisons *= gen_data->per_devices_gen_data[dev_idx].num_versions;
  size_t * version_combinaisons = (size_t *)malloc(num_version_combinaisons * acc_tuner->num_devices * sizeof(size_t));

  size_t version_combinaison_idx = 0;
  acc_tuning_gen_all_version_combinaisons(version_combinaisons, &version_combinaison_idx, 0, buffer, gen_data);
  assert(version_combinaison_idx == num_version_combinaisons);

  // Generate all combinaison of portions

  size_t num_portion_combinaisons = acc_tuning_count_portion_combinaisons(gen_data->num_portions, acc_tuner->num_devices);
  size_t * portion_combinaisons = (size_t *)malloc(num_portion_combinaisons * acc_tuner->num_devices * sizeof(size_t));

  size_t portion_combinaison_idx = 0;
  acc_tuning_gen_all_portion_combinaisons(gen_data->num_portions, 0, portion_combinaisons, &portion_combinaison_idx, buffer);
  assert(portion_combinaison_idx == num_portion_combinaisons);

  free(buffer);

  // Add valid runs to the DB

#if PRINT_INFO
  printf("[info]     #versions: %zd\n", num_version_combinaisons);
  printf("[info]     #portions: %zd\n", num_portion_combinaisons);
  printf("[info]     #params  : %zd\n", gen_data->num_params_values);
#endif

  char region_id_cond[20];
  sprintf(region_id_cond, "region_id == '%zd'", gen_data->region_id);
  char kernel_id_cond[20];
  sprintf(kernel_id_cond, "kernel_id == '%zd'", gen_data->kernel_id);

  size_t param_idx;
  for (version_combinaison_idx = 0; version_combinaison_idx < num_version_combinaisons; version_combinaison_idx++) {

#if PRINT_INFO
  printf("[info]       Next combinaison of versions... (%zd/%zd)\n", version_combinaison_idx, num_version_combinaisons);
#endif

    size_t * num_loops = malloc(acc_tuner->num_devices * sizeof(size_t));
    struct acc_sqlite_loop_entry_t ** loop_entries = malloc(acc_tuner->num_devices * sizeof(struct acc_sqlite_loop_entry_t *));
    for (dev_idx = 0; dev_idx < acc_tuner->num_devices; dev_idx++) {
      char version_id_cond[20];
      sprintf(version_id_cond, "version_id == '%zd'", *(version_combinaisons + acc_tuner->num_devices * version_combinaison_idx + dev_idx));
      char * loop_conds[3] = {region_id_cond, kernel_id_cond, version_id_cond};

      num_loops[dev_idx] = acc_sqlite_read_table(
                             acc_tuner->versions_db, "Loops",
                             3, loop_conds,
                             loop_entry_num_fields, loop_entry_field_names, loop_entry_field_types, loop_entry_field_sizes, loop_entry_field_offsets,
                             sizeof(struct acc_sqlite_loop_entry_t), (void**)&(loop_entries[dev_idx])
                           );
    }

    for (param_idx = 0; param_idx < gen_data->num_params_values; param_idx++) {
      for (portion_combinaison_idx = 0; portion_combinaison_idx < num_portion_combinaisons; portion_combinaison_idx++) {
        size_t num_gwv_combinaisons = 0;
        size_t (*gwv_combinaisons)[3] = malloc(num_gwv_combinaisons * acc_tuner->num_devices * sizeof(size_t[3]));

        acc_tuning_gen_valid_gwv_combinaisons(
          gen_data->params_values + param_idx * acc_tuner->params_size,
          version_combinaisons + acc_tuner->num_devices * version_combinaison_idx,
          portion_combinaisons + acc_tuner->num_devices * portion_combinaison_idx,
          &num_gwv_combinaisons, &gwv_combinaisons, num_loops, loop_entries, gen_data
        );

        size_t gwv_combinaison_idx;
        for (gwv_combinaison_idx = 0; gwv_combinaison_idx < num_gwv_combinaisons; gwv_combinaison_idx++) {
          struct acc_tuner_device_param_t * device_params = (struct acc_tuner_device_param_t *)malloc(acc_tuner->num_devices * sizeof(struct acc_tuner_device_param_t));
          for (dev_idx = 0; dev_idx < acc_tuner->num_devices; dev_idx++) {
            strcpy(device_params[dev_idx].device_name, acc_tuner->devices_name[dev_idx]);
            device_params[dev_idx].version_id = version_combinaisons[acc_tuner->num_devices * version_combinaison_idx + dev_idx];
            device_params[dev_idx].gang       = gwv_combinaisons[gwv_combinaison_idx][0];
            device_params[dev_idx].worker     = gwv_combinaisons[gwv_combinaison_idx][1];
            device_params[dev_idx].vector     = gwv_combinaisons[gwv_combinaison_idx][2];
            device_params[dev_idx].portion    = portion_combinaisons[acc_tuner->num_devices * portion_combinaison_idx + dev_idx];
          }

          acc_tuning_add_run(device_params, gen_data->params_values + param_idx * acc_tuner->params_size);
        }

        free(gwv_combinaisons);
      }
    }
  }

  free(portion_combinaisons);
  free(version_combinaisons);
}

int acc_tuning_create_experiment() {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_create_experiment(...)\n");
#endif
  char tmp[200];
  size_t i;

  assert(acc_tuner != NULL);

  char * experiment_desc = (char *)malloc(sizeof(char));
    experiment_desc[0] = '\0';
  size_t experiment_desc_length = 1;
  if (acc_tuner->num_devices == 1) {
    experiment_desc_length += sprintf(tmp, "version_id INT, acc_device_type CHAR(40), gang INT, worker INT, vector INT, ");
    experiment_desc = (char *)realloc(experiment_desc, experiment_desc_length * sizeof(char));
    strcat(experiment_desc, tmp);
  }    
  else if (acc_tuner->num_devices > 1) {
    for (i = 0; i < acc_tuner->num_devices; i++) {
      experiment_desc_length += sprintf(tmp, "version_id_%zd INT, acc_device_type_%zd CHAR(40), gang_%zd INT, worker_%zd INT, vector_%zd INT, portion_%zd INT, ", i, i, i, i, i, i);
      experiment_desc = (char *)realloc(experiment_desc, experiment_desc_length * sizeof(char));
      strcat(experiment_desc, tmp);
    }
  }
  else assert(0);

  struct acc_tuner_data_params_desc_t_ * data_params = acc_tuner->data_params;
  for (i = 0; i < data_params->num_params; i++) {      
    experiment_desc_length += sprintf(tmp, "%s %s, ", data_params->name_params[i], acc_sqlite_type_string(data_params->type_params[i]));
    experiment_desc = (char *)realloc(experiment_desc, experiment_desc_length * sizeof(char));
    strcat(experiment_desc, tmp);
  }

  char * exec_flag = " executed INT";
  experiment_desc_length += strlen(exec_flag);
  experiment_desc = (char *)realloc(experiment_desc, experiment_desc_length * sizeof(char));
  strcat(experiment_desc, exec_flag);

#if PRINT_INFO
  printf("[info]   Call acc_profiling_set_experiment(\"%s\")\n", experiment_desc);
#endif

  int res = acc_profiling_set_experiment(experiment_desc);

  free(experiment_desc);

  return res;
}

void acc_tuning_add_run(struct acc_tuner_device_param_t * device_params, void * params) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_add_run(...)\n");
#endif
  char tmp[200];
  size_t i;

  assert(acc_tuner != NULL);

  size_t run_desc_length = 1; // for final '\0' (even if char is NULL)
  char * run_desc = (char *)malloc(sizeof(char));
  run_desc[0] = '\0';
  if (acc_tuner->num_devices == 1) {
    run_desc_length += sprintf(tmp, "'%zd', '%s', '%zd', '%zd', '%zd', ",
                                    device_params->version_id, device_params->device_name,
                                    device_params->gang, device_params->worker, device_params->vector
                              );
    run_desc = (char *)realloc(run_desc, run_desc_length * sizeof(char));
    strcat(run_desc, tmp);
  }    
  else if (acc_tuner->num_devices > 1) {
    for (i = 0; i < acc_tuner->num_devices; i++) {
      run_desc_length += sprintf(tmp, "'%zd', '%s', '%zd', '%zd', '%zd', '%zd', ",
                                      device_params[i].version_id, device_params[i].device_name,
                                      device_params[i].gang, device_params[i].worker, device_params[i].vector,
                                      device_params[i].portion
                                );
      run_desc = (char *)realloc(run_desc, run_desc_length * sizeof(char));
      strcat(run_desc, tmp);
    }
  }
  else assert(0);

  for (i = 0; i < acc_tuner->data_params->num_params; i++) {
    switch (acc_tuner->data_params->type_params[i]) {
      case e_sqlite_int:
      {
        size_t param = *((size_t *)params);
        params += sizeof(size_t);
        run_desc_length += sprintf(tmp, "'%zd', ", param);
        run_desc = (char *)realloc(run_desc, run_desc_length * sizeof(char));
        strcat(run_desc, tmp);
        break;
      }
      case e_sqlite_float:
      {
        float param = *((float *)params);
        params += sizeof(float);
        run_desc_length += sprintf(tmp, "'%f', ", param);
        run_desc = (char *)realloc(run_desc, run_desc_length * sizeof(char));
        strcat(run_desc, tmp);
        break;
      }
      case e_sqlite_text:
      {
        char * param = *((char **)params);
        params += sizeof(char *);
        run_desc_length += sprintf(tmp, "'%s', ", param);
        run_desc = (char *)realloc(run_desc, run_desc_length * sizeof(char));
        strcat(run_desc, tmp);
        break;
      }
      default: assert(0);
    }
  }

  run_desc_length += sprintf(tmp, "'0'");
  run_desc = (char *)realloc(run_desc, run_desc_length * sizeof(char));
  strcat(run_desc, tmp);

#if PRINT_INFO
    printf("[info]   Call acc_profiling_new_run(\"%s\")\n", run_desc);
#endif

  acc_profiling_new_run(run_desc);

  free(run_desc);
}

struct acc_tuner_data_params_desc_t_ * acc_tuning_build_data_params(size_t num_params, ...) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_build_data_params(...)\n");
#endif
  struct acc_tuner_data_params_desc_t_ * result = (struct acc_tuner_data_params_desc_t_ *)malloc(sizeof(struct acc_tuner_data_params_desc_t_));
    result->name_params = (char **)malloc(num_params * sizeof(char *));
    result->type_params = (enum acc_sqlite_type_e *)malloc(num_params * sizeof(enum acc_sqlite_type_e));
    result->num_params = num_params;

  va_list args_ptr;
  va_start(args_ptr, num_params);

  size_t i;
  for (i = 0; i < num_params; i++) {
    char * param_name = va_arg(args_ptr, char *);
    result->name_params[i] = (char *)malloc((strlen(param_name) + 1) * sizeof(char));
    strcpy(result->name_params[i], param_name);

    enum acc_sqlite_type_e param_type = va_arg(args_ptr, enum acc_sqlite_type_e);
    result->type_params[i] = param_type;
  }
  va_end(args_ptr);

  return result;
}

void acc_tuner_exec_kernel(struct acc_tuner_exec_data_t * exec_data) {
  acc_region_start(exec_data->region);

  acc_enqueue_kernel(exec_data->region, exec_data->kernel);

  acc_region_stop(exec_data->region);

  acc_profiling_release_all_events();
}

void acc_tuning_execute(struct acc_tuner_exec_data_t * exec_data, ...) {
#if PRINT_INFO
  printf("[info]   Enter acc_tuning_execute(...)\n");
#endif
  assert(acc_tuner != NULL);

  size_t i, j;

  struct acc_region_desc_t_ * region = exec_data->region->desc;
  struct acc_kernel_desc_t_ * kernel = exec_data->kernel->desc;

  kernel->version_by_devices = malloc(acc_tuner->num_devices * sizeof(size_t));

  size_t num_conds = acc_tuner->num_devices + acc_tuner->data_params->num_params + 1;
  char ** conds = malloc(num_conds * sizeof(char *));

  region->num_options = acc_tuner->num_devices;
  region->options = malloc(acc_tuner->num_devices * sizeof(char *));

  va_list args_ptr;
  va_start(args_ptr, exec_data);
  if (acc_tuner->num_devices == 1) {
    kernel->version_by_devices[0] = va_arg(args_ptr, size_t);
    conds[0] = malloc(22 * sizeof(char));
    sprintf(conds[0], "version_id == '%zd'", kernel->version_by_devices[0]);

    /// \todo function for: lookup for acc_kernel_version_t_::id in acc_kernel_desc_t::versions
    struct acc_kernel_version_t_ * version = NULL;
    for (j = 0; j < kernel->num_versions; j++)
      if (kernel->versions[j]->id == kernel->version_by_devices[0]) {
        version = kernel->versions[j];
        break;
      }
    assert(version != NULL);

    region->options[0] = malloc((10 + strlen(kernel->name) + strlen(version->suffix)) * sizeof(char));
    sprintf(region->options[0], "-DENABLE_%s%s", kernel->name, version->suffix);
  }
  else {
    for (i = 0; i < acc_tuner->num_devices; i++) {
      kernel->version_by_devices[i] = va_arg(args_ptr, size_t);
      conds[i] = malloc(28 * sizeof(char));
      sprintf(conds[i], "version_id_%zd == '%zd'", i, kernel->version_by_devices[i]);

      struct acc_kernel_version_t_ * version = NULL;
      for (j = 0; j < kernel->num_versions; j++)
        if (kernel->versions[j]->id == kernel->version_by_devices[i]) {
          version = kernel->versions[j];
          break;
        }
      assert(version != NULL);

      region->options[i] = malloc((11 + strlen(kernel->name) + strlen(version->suffix)) * sizeof(char));
      sprintf(region->options[i], "-DENABLE_%s%s ", kernel->name, version->suffix);
    }
  }
  for (i = 0; i < acc_tuner->data_params->num_params; i++) {
    size_t idx = acc_tuner->num_devices + i;
    switch (acc_tuner->data_params->type_params[i]) {
      case e_sqlite_int:
      {
        size_t param = va_arg(args_ptr, size_t);
        conds[idx] = malloc((strlen(acc_tuner->data_params->name_params[i]) + 24) * sizeof(char));
        sprintf(conds[idx], "%s == '%zd'", acc_tuner->data_params->name_params[i], param);
        break;
      }
      case e_sqlite_float:
      {
        double param = va_arg(args_ptr, double);
        conds[idx] = malloc((strlen(acc_tuner->data_params->name_params[i]) + 24) * sizeof(char));
        sprintf(conds[idx], "%s == '%f'", acc_tuner->data_params->name_params[i], param);
        break;
      }
      case e_sqlite_text:
      {
        char * param = va_arg(args_ptr, char *);
        conds[idx] = malloc((strlen(acc_tuner->data_params->name_params[i]) + strlen(param) + 7) * sizeof(char));
        sprintf(conds[idx], "%s == '%s'", acc_tuner->data_params->name_params[i], param);
        break;
      }
      default:
        assert(0);
    }
  }
  va_end(args_ptr);

  conds[num_conds - 1] = "executed == '1'";

  size_t entry_size;
  void * run_entries;
  size_t num_fields;
  char ** field_names;
  enum acc_sqlite_type_e * field_types;
  size_t * field_sizes;
  size_t * field_offsets;

  size_t num_candidates = acc_sqlite_read_run_table(
                            acc_profiler->db_file, acc_tuner->num_devices, acc_tuner->data_params, num_conds, conds,
                            &entry_size, &run_entries, &num_fields, &field_names, &field_types, &field_sizes, &field_offsets, NULL
                          );

  free(conds);

#if PRINT_INFO
  printf("[info]     num_candidates = %zd\n", num_candidates);
#endif

  acc_push_data_environment();

  for (i = 0; i < exec_data->num_data_in; i++)
    acc_present_or_copyin_regions_(
        exec_data->region,
        exec_data->kernel->data_ptrs[exec_data->data_in[i]],
        exec_data->kernel->data_size[exec_data->data_in[i]]
    );
  for (i = 0; i < exec_data->num_data_create; i++)
    acc_present_or_create_regions_(
        exec_data->region,
        exec_data->kernel->data_ptrs[exec_data->data_create[i]],
        exec_data->kernel->data_size[exec_data->data_create[i]]
    );

  for (i = 0; i < num_candidates; i++) {
    void * run_entry = run_entries + i * entry_size;
    size_t run_id =  *(size_t *)run_entry;
    for (j = 0; j < acc_tuner->num_devices; j++) {
      exec_data->region->devices[j].num_gang      = *(size_t *)(run_entry + field_offsets[6 * j + 3]);
      exec_data->region->devices[j].num_worker    = *(size_t *)(run_entry + field_offsets[6 * j + 4]);
      exec_data->region->devices[j].vector_length = *(size_t *)(run_entry + field_offsets[6 * j + 5]);
#if PRINT_INFO
      printf("[info]       Device %zd uses gang = %zd, worker = %zd, and vector = %zd\n", j, exec_data->region->devices[j].num_gang, exec_data->region->devices[j].num_worker, exec_data->region->devices[j].vector_length);
#endif
    }

    acc_profiler->run_id = run_id;

    char * err_msg;
    int status;
    char * query = malloc(64 * sizeof(char));

    sprintf(query, "UPDATE Runs SET executed = '2' WHERE rowid == '%zd';", run_id);
    status = sqlite3_exec (acc_profiler->db_file, query, NULL, 0, &err_msg);
    assert(status == SQLITE_OK);
    
    acc_tuner_exec_kernel(exec_data);

    sprintf(query, "UPDATE Runs SET executed = '3' WHERE rowid == '%zd';", run_id);
    status = sqlite3_exec (acc_profiler->db_file, query, NULL, 0, &err_msg);
    assert(status == SQLITE_OK);
  }

  for (i = 0; i < exec_data->num_data_out; i++)
    acc_present_or_copyout_regions_(
        exec_data->region,
        exec_data->kernel->data_ptrs[exec_data->data_out[i]],
        exec_data->kernel->data_size[exec_data->data_out[i]]
    );

  acc_pop_data_environment();

  acc_sqlite_clean_run_table_read(acc_tuner->num_devices, field_names, field_types, field_sizes, field_offsets);
  free(run_entries);
}

