
#ifndef _BIMAP_H_
#define _BIMAP_H_

#include "OpenACC/utils/containers/map.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bimap_t_
{
    /// size of datas 
    size_t size;
    /// Number of stored datas
    size_t count;
    /// Size of one data
    size_t data_size;
    /// Size of Key #1
    size_t key_1_size;
    /// Size of Key #2
    size_t key_2_size;
    /// size of storage for each data 
    size_t storage_size;

    /// Map from Key #1 to the index of the corresponding value in values
    map_t key_1_map;
    /// Map from Key #2 to the index of the corresponding value in values
    map_t key_2_map;

    /// Array that stores the mapped values
    void * datas;
};
typedef struct bimap_t_* bimap_t;

bimap_t bimap_alloc(size_t size, size_t key_1_size, size_t key_2_size, size_t data_size, key_cmp_f less_1, key_cmp_f less_2);

void bimap_insert(bimap_t bimap, const key_type * k1, const key_type * k2, const data_type * data);

size_t bimap_get_index_by_key_1(bimap_t bimap, const key_type * k1);
size_t bimap_get_index_by_key_2(bimap_t bimap, const key_type * k2);

const data_type * bimap_get_value_by_key_1(bimap_t bimap, const key_type * k1);
const data_type * bimap_get_value_by_key_2(bimap_t bimap, const key_type * k2);

void bimap_remove_by_key_1(bimap_t bimap, const key_type * k1);
void bimap_remove_by_key_2(bimap_t bimap, const key_type * k2);

void bimap_free(bimap_t bimap);

#ifdef __cplusplus
}
#endif

#endif /* _BIMAP_H_ */

