#ifndef _MAP_H_
#define _MAP_H_

#include "OpenACC/utils/sorted_list.h"
#include "OpenACC/utils/linked_list.h"

struct
pair_t_
{
    void* key;
    void* value;
};
typedef struct pair_t_ pair_t;

struct
map_t_
{
    sorted_list_t values;
    linked_list_t data;
};
typedef struct map_t_* map_t;

map_t
map_create (sorted_list_comparator_t);

void
map_insert (map_t, void*, void*);

void
map_remove (map_t, void*);

void*
map_get (map_t, void*);

void
map_clear (map_t);

#endif /* _MAP_H_ */
