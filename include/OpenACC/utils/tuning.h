
#ifndef __LIBOPENACC_TUNING_H__
#define __LIBOPENACC_TUNING_H__

#include "OpenACC/openacc.h"

#include "OpenACC/utils/sqlite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t (*acc_tuner_loop_length_func_t)(size_t, void *);

/// Different combinaisons to be tested
struct acc_tuner_device_ranges_t_ {
  size_t num_versions;
  size_t num_gang_values;
  size_t num_worker_values;
  size_t num_vector_values;

  size_t * version_ids;
  size_t * gang_values;
  size_t * worker_values;
  size_t * vector_values;
};

/// Create a descriptor of the data for which we tuned a given kernel
struct acc_tuner_device_ranges_t_ * acc_tuning_build_device_ranges(size_t num_devices);

/// Parameters space
struct acc_tuner_data_params_desc_t_ {
  char ** name_params;                  /// Names used to store the params in the database
  enum acc_sqlite_type_e * type_params; /// Type of the params (sqlite types): INT, FLOAT, TEXT (C types are respectively size_t, float, char *)
  size_t num_params;                    /// Number of params
};

/// Create a descriptor of the data for which we tuned a given kernel 
struct acc_tuner_data_params_desc_t_ * acc_tuning_build_data_params(size_t num_params, ...);

struct acc_tuner_t {
  size_t num_devices;
  char ** devices_name;
  struct acc_tuner_device_ranges_t_ * ranges_per_devices;

  struct acc_tuner_data_params_desc_t_ * data_params;
  size_t params_size;
  size_t num_params_values;
  void * params_values;

  size_t num_portions;

  acc_tuner_loop_length_func_t loop_length_func;

  sqlite3 * versions_db;
};
extern struct acc_tuner_t * acc_tuner;

/**
 *  Initialize the libOpenACC tuner's
 */
int acc_tuning_init(
  size_t num_devices,
  char ** devices_name,
  struct acc_tuner_device_ranges_t_ * ranges_per_devices,
  struct acc_tuner_data_params_desc_t_ * data_params,
  size_t num_params_values,
  void * params_values,
  size_t num_portions,
  acc_tuner_loop_length_func_t loop_length_func,
  sqlite3 * versions_db
);


struct acc_tuner_exec_data_t {
  struct acc_region_t_ * region;
  struct acc_kernel_t_ * kernel;

  size_t num_data_in;
  size_t * data_in;

  size_t num_data_out;
  size_t * data_out;

  size_t num_data_create;
  size_t * data_create;
};

void acc_tuning_execute(struct acc_tuner_exec_data_t * exec_data, ...);

#ifdef __cplusplus
}
#endif


#endif /* __LIBOPENACC_TUNING_H__ */

