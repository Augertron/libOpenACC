#ifndef _MAP_H_
#define _MAP_H_

#include "OpenACC/utils/containers/associative.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct associative_t_ * map_t;

map_t map_alloc(size_t size, size_t key_size, size_t data_size, key_cmp_f less);

void map_insert(map_t map, const key_type * key, const data_type * data);

void map_remove(map_t map, const key_type * key);

const void * map_lookup(map_t map, const key_type * key);

void map_clear(map_t map);

void map_free(map_t map);

#ifdef __cplusplus
}
#endif

#endif /* _MAP_H_ */
