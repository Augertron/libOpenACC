
#include "OpenACC/utils/containers/map.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

map_t map_alloc(size_t size, size_t key_size, size_t data_size, key_cmp_f less) {
  return associative_alloc(size, key_size, data_size, less);
}

void map_insert(map_t map, const key_type * key, const data_type * data) {
  associative_insert(map, key, data);
}

void map_remove(map_t map, const key_type * key) {
  associative_remove(map, key);
}

void * map_lookup(map_t map, const key_type * key) {
  return associative_lookup(map, key);
}

void map_free(map_t map) {
  associative_free(map);
}

