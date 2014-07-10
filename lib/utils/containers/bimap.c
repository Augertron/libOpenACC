
#include "OpenACC/utils/containers/bimap.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

bimap_t bimap_alloc(size_t size, size_t key_1_size, size_t key_2_size, size_t data_size, key_cmp_f less_1, key_cmp_f less_2) {
  bimap_t bimap = (bimap_t)malloc(sizeof(struct bimap_t_));

  bimap->size = size;
  bimap->data_size = data_size;
  bimap->key_1_size = key_1_size;
  bimap->key_2_size = key_2_size;
  bimap->storage_size = data_size + key_1_size + key_2_size;
  bimap->count = 0;

  bimap->key_1_map = map_alloc(size, key_1_size, sizeof(size_t), less_1);
  bimap->key_2_map = map_alloc(size, key_2_size, sizeof(size_t), less_2);

  bimap->datas = malloc(size * bimap->storage_size);

  return bimap;
}

void bimap_insert(bimap_t bimap, const key_type * k1, const key_type * k2, const data_type * data) {
  size_t key_1_idx = bimap_get_index_by_key_1(bimap, k1);
  size_t key_2_idx = bimap_get_index_by_key_2(bimap, k2);

  assert(key_1_idx == -1); /// Trying to insert with an existing key is an error (#1)
  assert(key_2_idx == -1); /// Trying to insert with an existing key is an error (#2)

  if (bimap->count == bimap->size) {
    /// \todo realloc

    assert(0);
  }

  memcpy(bimap->datas + bimap->count * bimap->storage_size                                       , data, bimap->data_size );
  memcpy(bimap->datas + bimap->count * bimap->storage_size + bimap->data_size                    , k1  , bimap->key_1_size);
  memcpy(bimap->datas + bimap->count * bimap->storage_size + bimap->data_size + bimap->key_1_size, k2  , bimap->key_2_size);

  map_insert(bimap->key_1_map, k1, &(bimap->count));
  map_insert(bimap->key_2_map, k2, &(bimap->count));

  bimap->count++;
}

size_t bimap_get_index_by_key_1(bimap_t bimap, const key_type * k1) {
  size_t * idx = map_lookup(bimap->key_1_map, k1);

  if (idx == NULL) return -1;

  return *idx;
}

size_t bimap_get_index_by_key_2(bimap_t bimap, const key_type * k2) {
  size_t * idx = map_lookup(bimap->key_2_map, k2);

  if (idx == NULL) return -1;

  return *idx;
}

const data_type * bimap_get_value_by_key_1(bimap_t bimap, const key_type * k1) {
  size_t * idx = map_lookup(bimap->key_1_map, k1);

  if (idx == NULL) return NULL;

  return bimap->datas + *idx * bimap->storage_size;
}

const data_type * bimap_get_value_by_key_2(bimap_t bimap, const key_type * k2) {
  size_t * idx = map_lookup(bimap->key_2_map, k2);

  if (idx == NULL) return NULL;

  return bimap->datas + *idx * bimap->storage_size;
}

void bimap_remove(bimap_t bimap, const key_type * k1, const key_type * k2, size_t idx) {
  /*printf("[debug] bimap_remove(bimap = %x, *k1 = %x, *k2 = %x, idx = %zd)\n", bimap, *(void**)k1, *(void**)k2, idx);
  printf("[debug]     bimap->count = %zd\n", bimap->count);
  printf("[debug]     bimap->key_1_map->count = %zd\n", bimap->key_1_map->count);
  printf("[debug]     bimap->key_2_map->count = %zd\n", bimap->key_2_map->count);*/

  size_t i;
  /*printf("[debug]     Start:\n");
  for (i = 0; i < bimap->count; i++) {
    printf("[debug]       #%zd : { {%x,%x,%zd}, {%x,%zd}, {%x,%zd} }\n", i,
      *(void**)(bimap->datas + i * bimap->storage_size),
      *(void**)(bimap->datas + i * bimap->storage_size + sizeof(void*)),
      *(size_t*)(bimap->datas + i * bimap->storage_size + 2 * sizeof(void*)),
      *(void**)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size),
      *(size_t*)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size + bimap->key_1_map->key_size),
      *(void**)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size),
      *(size_t*)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size + bimap->key_2_map->key_size)
    );
  }*/

  map_remove(bimap->key_1_map, k1);

  /*printf("[debug]     Map key 1 after removal:\n");
  for (i = 0; i < bimap->key_1_map->count; i++) {
    printf("[debug]       #%zd : {%x,%zd}\n", i,
      *(void**)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size),
      *(size_t*)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size + bimap->key_1_map->key_size)
    );
  }*/

  assert(bimap->key_1_map->count == bimap->count - 1);
  for (i = 0; i < bimap->key_1_map->count; i++) {
    size_t * data_idx = bimap->key_1_map->datas + i * bimap->key_1_map->storage_size + bimap->key_1_map->key_size;
//  assert(*(void**)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size) != *(void**)k1);
    assert(*data_idx != idx);
    if (*data_idx > idx)
      (*data_idx) -= 1;
  }

  map_remove(bimap->key_2_map, k2);

  /*printf("[debug]     Map key 2 after removal:\n");
  for (i = 0; i < bimap->key_2_map->count; i++) {
    printf("[debug]       #%zd : {%x,%zd}\n", i,
      *(void**)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size),
      *(size_t*)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size + bimap->key_2_map->key_size)
    );
  }*/

  assert(bimap->key_2_map->count == bimap->count - 1);
  for (i = 0; i < bimap->key_2_map->count; i++) {
    size_t * data_idx = bimap->key_2_map->datas + i * bimap->key_2_map->storage_size + bimap->key_2_map->key_size;
//  assert(*(void**)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size) != *(void**)k2);
    assert(*data_idx != idx);
    if (*data_idx > idx)
      (*data_idx) -= 1;
  }

  bimap->count--;
  memmove(bimap->datas + idx * bimap->storage_size, bimap->datas + (idx + 1) * bimap->storage_size, bimap->count * bimap->storage_size);

  /*printf("[debug]     End:\n");
  for (i = 0; i < bimap->count; i++) {
    printf("[debug]       #%zd : { {%x,%x,%zd}, {%x,%zd}, {%x,%zd} }\n", i,
      *(void**)(bimap->datas + i * bimap->storage_size),
      *(void**)(bimap->datas + i * bimap->storage_size + sizeof(void*)),
      *(size_t*)(bimap->datas + i * bimap->storage_size + 2 * sizeof(void*)),
      *(void**)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size),
      *(size_t*)(bimap->key_1_map->datas + i * bimap->key_1_map->storage_size + bimap->key_1_map->key_size),
      *(void**)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size),
      *(size_t*)(bimap->key_2_map->datas + i * bimap->key_2_map->storage_size + bimap->key_2_map->key_size)
    );
  }*/
}

void bimap_remove_by_key_1(bimap_t bimap, const key_type * k1) {
  size_t * idx = map_lookup(bimap->key_1_map, k1);

  if (idx == NULL) return;

  const key_type * k2 = bimap->datas + *idx * bimap->storage_size + bimap->data_size + bimap->key_1_size;

  bimap_remove(bimap, k1, k2, *idx);
}

void bimap_remove_by_key_2(bimap_t bimap, const key_type * k2) {
  size_t * idx = map_lookup(bimap->key_2_map, k2);

  if (idx == NULL) return;

  const key_type * k1 = bimap->datas + *idx * bimap->storage_size + bimap->data_size;

  bimap_remove(bimap, k1, k2, *idx);
}

void bimap_free(bimap_t bimap) {
  map_free(bimap->key_1_map);
  map_free(bimap->key_2_map);
  free(bimap->datas);
  free(bimap);
}

