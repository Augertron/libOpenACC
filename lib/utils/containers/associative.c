
#include "OpenACC/utils/containers/associative.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#ifndef PRINT_INFO
# define PRINT_INFO 0
#endif
typedef struct associative_t_ * associative_t;

inline const key_type * associative_get_key_by_index(struct associative_t_ * container, size_t idx) {
  return container->datas + idx * container->storage_size;
}

inline const data_type * associative_get_data_by_index(struct associative_t_ * container, size_t idx) {
  return container->datas + idx * container->storage_size + container->key_size;
}

inline void * associative_get_storage_by_index(struct associative_t_ * container, size_t idx) {
  return container->datas + idx * container->storage_size;
}

/*! 
 *  \brief (internal) Recursive lookup function for associative containers.
 *
 *  \param data_base pointer to an array (or sub-array) of datas
 *  \param data pointer to the data we
 *
 *  \return pointer to the largest element <= than data, max({x in datas | x <= data})
 */
size_t associative_lookup_(
  struct associative_t_ * container,
  const key_type * key,
  size_t start,
  size_t n
) {
#if PRINT_INFO
  printf("[info]    associative_lookup_(container = %x, key = %x, start = %zd, n = %zd) with container->count = %u\n",
         container, *((void**)key), start, n, container->count);
#endif

  assert(n > 0);

  assert(start + n <= container->count);

  if (n == 1) {
    const key_type * key_ = associative_get_key_by_index(container, start);
    if ((*container->key_less)(key, key_))
      return start - 1;
    else
      return start;
  }
  else if (n == 2) {
    const key_type * key_0 = associative_get_key_by_index(container, start);
    const key_type * key_1 = associative_get_key_by_index(container, start + 1);

    if ((*container->key_less)(key, key_0))
      return start - 1;
    else if ((*container->key_less)(key, key_1))
      return start;
    else
      return start + 1;
  }
  else {
    size_t pivot = n/2;

#if PRINT_INFO
    printf("[info]      associative_lookup_(...) : pivot = %zd\n", pivot);
#endif

    const key_type * key_ = associative_get_key_by_index(container, start + pivot);
    if ((*container->key_less)(key, key_))                                          // key < key[start + pivot]
      return associative_lookup_(container, key, start, pivot);                       // search in [ start , start+pivot [
    else if ((*container->key_less)(key_, key))                                     // key > key[start + pivot]
      return associative_lookup_(container, key, start + pivot + 1, n - pivot - 1);   // search in ] start+pivot , start+n [
    else                                                                            // key == key[start + pivot]
      return pivot;                                                                   // Found!
  }
}

/*! 
 *  \brief (internal) remove key/data from the container
 */
void associative_remove_(
  struct associative_t_ * container,
  size_t idx
) {
#if PRINT_INFO
  printf("[info]    associative_remove_(container = %x, idx = %zd) with container->count = %u\n", container, idx, container->count);
#endif

  void * storage_start = associative_get_storage_by_index(container, idx);
  void * storage_stop  = storage_start + container->storage_size;

  void * container_end = container->datas + container->count * container->storage_size;

  ptrdiff_t tail_size = container_end - storage_stop;

  memmove(storage_start, storage_stop, tail_size);

  container->count--;
}

/*! 
 *  \brief (internal) Insert a new element in the container after the specified data (used by associative_insert_after_key)
 *  Can be used with the result of associative_lookup on the same container iff the container have not been modified since this call
 */
void associative_insert_(
  struct associative_t_ * container,
  size_t idx,
  const key_type * key,
  const data_type * data
) {
#if PRINT_INFO
  printf("[info]    associative_insert_(container = %x, idx = %zd, key = %x, data = ...) with container->count = %u\n",
         container, idx, *((void**)key), container->count);
#endif

  // Check that we have enougth space
  if (container->count == container->size) {
    container->datas = realloc(container->datas, 2 * container->size * container->storage_size);
    assert(container->datas != NULL);
    container->size = 2 * container->size;
  }

  void * container_end = container->datas + container->count * container->storage_size;
  if (idx == -1) {
    memmove(container->datas + container->storage_size, container->datas, container->count * container->storage_size);

    memcpy(container->datas, key, container->key_size);

    if (container->data_size > 0)
      memcpy(container->datas + container->key_size, data, container->data_size);
  }
  else if (idx == container->count) {
#if PRINT_INFO
    printf("[info]      associative_lookup_(...) : idx == container->count\n");
    memcpy(container_end, key, container->key_size);
#endif

    if (container->data_size > 0)
      memcpy(container_end + container->key_size, data, container->data_size);
  }
  else {
    void * storage_start = associative_get_storage_by_index(container, idx+1);
    void * storage_stop  = storage_start + container->storage_size;

    ptrdiff_t tail_size = container_end - storage_start;

    memmove(storage_stop, storage_start, tail_size);

    memcpy(storage_start, key, container->key_size);

    if (container->data_size > 0)
      memcpy(storage_start + container->key_size, data, container->data_size);
  }
  container->count++;
}

associative_t associative_alloc(size_t size, size_t key_size, size_t data_size, key_cmp_f key_less) {
  associative_t container = (associative_t)malloc(sizeof(struct associative_t_));

  container->size = size;
  container->count = 0;

  container->key_size = key_size;
  assert(container->key_size > 0);

  container->data_size = data_size;

  container->storage_size = key_size + data_size;
/*!
 * \note storage size can be used to align entries
 *
  if (container->storage_size % 16 != 0) {
    size_t padding = 16 - (container->storage_size % 16)
    container->storage_size = container->storage_size + padding;
  }
 *
 */

  container->key_less = key_less;
  assert(container->key_less != NULL);

  container->datas = malloc(size * container->storage_size);
  assert(container->datas != NULL);

  return container;
}

const data_type * associative_lookup(
  struct associative_t_ * container,
  const key_type * key
) {
  if (container->count == 0) return NULL; // Not found

  size_t idx = associative_lookup_(container, key, 0, container->count);

  if (idx < 0 || idx >= container->count) return NULL; // Not found

  key_type * found_key = associative_get_key_by_index(container, idx);
  if ( (*container->key_less)(found_key, key) || (*container->key_less)(key, found_key) ) return NULL; // Not found

  return associative_get_data_by_index(container, idx);
}

void associative_remove(
  struct associative_t_ * container,
  const key_type * key
) {
  size_t idx = -1;
  if (container->count > 0) idx = associative_lookup_(container, key, 0, container->count);

  if (idx < 0 || idx >= container->count) return; // Not found

  key_type * found_key = associative_get_key_by_index(container, idx);
  if ( (*container->key_less)(found_key, key) || (*container->key_less)(key, found_key) ) return; // Not found

  associative_remove_(container, idx);
}

void associative_insert(
  struct associative_t_ * container,
  const key_type * key,
  const data_type * data
) {
  size_t idx = -1;
  if (container->count > 0) idx = associative_lookup_(container, key, 0, container->count);
  if (idx >= 0 && idx < container->count) {
    key_type * found_key = associative_get_key_by_index(container, idx);
    if ( !(*container->key_less)(found_key, key) && !(*container->key_less)(key, found_key) ) return; // Key already exist
  }

  associative_insert_(container, idx, key, data);
}

void associative_clear(struct associative_t_ * container) {
  container->count = 0;
}

void associative_free(struct associative_t_ * container) {
  free(container->datas);
  free(container);
}

