/*!
 * \file OpenACC/utils/set.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_UTILS_ASSOCIATIVE
#define LIBOPENACC_UTILS_ASSOCIATIVE 20131111

#include "OpenACC/utils/containers/def.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct associative_t_ {
  size_t size;
  size_t count;

  size_t key_size;
  size_t data_size;
  size_t storage_size;

  key_cmp_f key_less;

  /// \brief struct { key_type key; data_type data; } datas[size];
  void * datas;
};

struct associative_t_ * associative_alloc(size_t size, size_t key_size, size_t data_size, key_cmp_f less);

/*! 
 *  \brief Lookup function for associative containers
 */
const data_type * associative_lookup(
  struct associative_t_ * container,
  const key_type * key
);

/*! 
 *  \brief Lookup function for associative containers
 */
void associative_remove(
  struct associative_t_ * container,
  const key_type * key
);

/*! 
 *  \brief Insert a data into an associative container based on its key
 */
void associative_insert(
  struct associative_t_ * container,
  const key_type * key,
  const data_type * data
);

void associative_free(struct associative_t_ * container);

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_UTILS_ASSOCIATIVE */

