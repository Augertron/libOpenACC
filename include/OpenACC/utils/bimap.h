#ifndef _BIMAP_H_
#define _BIMAP_H_

#include "OpenACC/utils/linked_list.h"
#include "OpenACC/utils/map.h"

struct
bimap_t_
{
    linked_list_t data;
    map_t key_map;
    map_t value_map;
    size_t count;
};
typedef struct bimap_t_* bimap_t;

bimap_t
bimap_create (sorted_list_comparator_t, sorted_list_comparator_t);

void
bimap_insert (bimap_t, void*, void*);

void*
bimap_get_by_key (bimap_t, void*);

void*
bimap_get_by_value (bimap_t, void*);

void
bimap_remove_by_key (bimap_t, void*);

void
bimap_remove_by_value (bimap_t, void*);

void
bimap_clear (bimap_t);

#endif /* _BIMAP_H_ */
