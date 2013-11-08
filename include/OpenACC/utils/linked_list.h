#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "OpenACC/utils/node.h"

#include <stddef.h>

typedef int (*linked_list_comparator_t) (void*, void*);

struct
linked_list_t_
{
    node_t data;
    size_t count;
};
typedef struct linked_list_t_* linked_list_t;

linked_list_t
list_create ();

void
list_free (linked_list_t);

void
list_insert_front (linked_list_t, node_t);

void
list_emplace_front (linked_list_t, void*);

void
list_insert_back (linked_list_t, node_t);

void
list_emplace_back (linked_list_t, void*);

node_t
list_find (linked_list_t, void*, linked_list_comparator_t);

void
list_remove_value (linked_list_t, void*, linked_list_comparator_t);

void
list_remove_node (linked_list_t, node_t);

size_t
list_count (linked_list_t);

unsigned char
list_empty (linked_list_t);

void
list_clear (linked_list_t);

#endif /* _LINKED_LIST_H_ */
