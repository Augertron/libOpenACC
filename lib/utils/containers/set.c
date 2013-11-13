
#include "OpenACC/utils/containers/set.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

set_t set_alloc(size_t size, size_t key_size, key_cmp_f less) {
  return associative_alloc(size, key_size, 0, less);
}

void set_insert(set_t set, const key_type * key) {
  associative_insert(set, key, NULL);
}

void set_remove(set_t set, const key_type * key) {
  associative_remove(set, key);
}

const key_type * set_lookup(set_t set, const key_type * key) {
  return associative_lookup(set, key) - set->key_size;
}

void set_free(set_t set) {
  associative_free(set);
}

